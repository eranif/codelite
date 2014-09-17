# -*- coding: iso-8859-1 -*-
# Pretty-printers for Qt4.

# Copyright (C) 2009 Niko Sams <niko.sams@gmail.com>

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import gdb
import itertools
import re

from helper import *

class QStringPrinter:

    def __init__(self, val):
        self.val = val

    def to_string(self):
        size = self.val['d']['size']
        ret = ""

        # The QString object might be not yet initialized. In this case size is a bogus value
        # and the following 2 lines might throw memory access error. Hence the try/catch.
        try:
            isQt4 = has_field(self.val['d'], 'data') # Qt4 has d->data, Qt5 doesn't.
            if isQt4:
                dataAsCharPointer = self.val['d']['data'].cast(gdb.lookup_type("char").pointer())
            else:
                dataAsCharPointer = (self.val['d'] + 1).cast(gdb.lookup_type("char").pointer())
            ret = dataAsCharPointer.string(encoding = 'UTF-16', length = size * 2)
        except Exception:
            # swallow the exception and return empty string
            pass
        return ret

    def display_hint (self):
        return 'string'


class QByteArrayPrinter:

    def __init__(self, val):
        self.val = val

    class _iterator(Iterator):
        def __init__(self, data, size):
            self.data = data
            self.size = size
            self.count = 0

        def __iter__(self):
            return self

        def __next__(self):
            if self.count >= self.size:
                raise StopIteration
            count = self.count
            self.count = self.count + 1
            return ('[%d]' % count, self.data[count])

    def children(self):
        return self._iterator(self.val['d']['data'], self.val['d']['size'])

    def to_string(self):
        #todo: handle charset correctly
        return self.val['d']['data'].string()

    def display_hint (self):
        return 'string'

class QListPrinter:
    "Print a QList"

    class _iterator(Iterator):
        def __init__(self, nodetype, d):
            self.nodetype = nodetype
            self.d = d
            self.count = 0

        def __iter__(self):
            return self

        def __next__(self):
            if self.count >= self.d['end'] - self.d['begin']:
                raise StopIteration
            count = self.count
            array = self.d['array'][self.d['begin'] + count]

            #from QTypeInfo::isLarge
            isLarge = self.nodetype.sizeof > gdb.lookup_type('void').pointer().sizeof

            isPointer = self.nodetype.code == gdb.TYPE_CODE_PTR

            #unfortunately we can't use QTypeInfo<T>::isStatic as it's all inlined, so use
            #this list of types that use Q_DECLARE_TYPEINFO(T, Q_MOVABLE_TYPE)
            #(obviously it won't work for custom types)
            movableTypes = ['QRect', 'QRectF', 'QString', 'QMargins', 'QLocale', 'QChar', 'QDate', 'QTime', 'QDateTime', 'QVector',
               'QRegExpr', 'QPoint', 'QPointF', 'QByteArray', 'QSize', 'QSizeF', 'QBitArray', 'QLine', 'QLineF', 'QModelIndex', 'QPersitentModelIndex',
               'QVariant', 'QFileInfo', 'QUrl', 'QXmlStreamAttribute', 'QXmlStreamNamespaceDeclaration', 'QXmlStreamNotationDeclaration',
               'QXmlStreamEntityDeclaration']
            #this list of types that use Q_DECLARE_TYPEINFO(T, Q_PRIMITIVE_TYPE) (from qglobal.h)
            primitiveTypes = ['bool', 'char', 'signed char', 'unsigned char', 'short', 'unsigned short', 'int', 'unsigned int', 'long', 'unsigned long', 'long long', 'unsigned long long', 'float', 'double']

            if movableTypes.count(self.nodetype.tag) or primitiveTypes.count(str(self.nodetype)):
               isStatic = False
            else:
                isStatic = not isPointer

            if isLarge or isStatic: #see QList::Node::t()
                node = array.cast(gdb.lookup_type('QList<%s>::Node' % self.nodetype).pointer())
            else:
                node = array.cast(gdb.lookup_type('QList<%s>::Node' % self.nodetype))
            self.count = self.count + 1
            return ('[%d]' % count, node['v'].cast(self.nodetype))

    def __init__(self, val, container, itype):
        self.val = val
        self.container = container
        if itype == None:
            self.itype = self.val.type.template_argument(0)
        else:
            self.itype = gdb.lookup_type(itype)

    def children(self):
        return self._iterator(self.itype, self.val['d'])

    def to_string(self):
        if self.val['d']['end'] == self.val['d']['begin']:
            empty = "empty "
        else:
            empty = ""

        return "%s%s<%s>" % ( empty, self.container, self.itype )

class QVectorPrinter:
    "Print a QVector"

    class _iterator(Iterator):
        def __init__(self, nodetype, d, p):
            self.nodetype = nodetype
            self.d = d
            self.p = p
            self.count = 0

        def __iter__(self):
            return self

        def __next__(self):
            if self.count >= self.p['size']:
                raise StopIteration
            count = self.count

            self.count = self.count + 1
            return ('[%d]' % count, self.p['array'][count])

    def __init__(self, val, container):
        self.val = val
        self.container = container
        self.itype = self.val.type.template_argument(0)

    def children(self):
        return self._iterator(self.itype, self.val['d'], self.val['p'])

    def to_string(self):
        if self.val['d']['size'] == 0:
            empty = "empty "
        else:
            empty = ""

        return "%s%s<%s>" % ( empty, self.container, self.itype )

class QLinkedListPrinter:
    "Print a QLinkedList"

    class _iterator(Iterator):
        def __init__(self, nodetype, begin, size):
            self.nodetype = nodetype
            self.it = begin
            self.pos = 0
            self.size = size

        def __iter__(self):
            return self

        def __next__(self):
            if self.pos >= self.size:
                raise StopIteration

            pos = self.pos
            val = self.it['t']
            self.it = self.it['n']
            self.pos = self.pos + 1
            return ('[%d]' % pos, val)

    def __init__(self, val):
        self.val = val
        self.itype = self.val.type.template_argument(0)

    def children(self):
        return self._iterator(self.itype, self.val['e']['n'], self.val['d']['size'])

    def to_string(self):
        if self.val['d']['size'] == 0:
            empty = "empty "
        else:
            empty = ""

        return "%sQLinkedList<%s>" % ( empty, self.itype )

class QMapPrinter:
    "Print a QMap"

    class _iterator(Iterator):
        def __init__(self, val):
            self.val = val
            self.ktype = self.val.type.template_argument(0)
            self.vtype = self.val.type.template_argument(1)
            self.data_node = self.val['e']['forward'][0]
            self.count = 0

        def __iter__(self):
            return self

        def payload (self):
            if gdb.parse_and_eval:
                ret = int(gdb.parse_and_eval('QMap<%s, %s>::payload()' % (self.ktype, self.vtype)))
                if (ret): return ret;
            
            #if the inferior function call didn't work, let's try to calculate ourselves

            #we can't use QMapPayloadNode as it's inlined
            #as a workaround take the sum of sizeof(members)
            ret = self.ktype.sizeof
            ret += self.vtype.sizeof
            ret += gdb.lookup_type('void').pointer().sizeof

            #but because of data alignment the value can be higher
            #so guess it's aliged by sizeof(void*)
            #TODO: find a real solution for this problem
            ret += ret % gdb.lookup_type('void').pointer().sizeof

            #for some reason booleans are different
            if str(self.vtype) == 'bool':
                ret += 2

            ret -= gdb.lookup_type('void').pointer().sizeof
            
            return ret

        def concrete (self, data_node):
            node_type = gdb.lookup_type('QMapNode<%s, %s>' % (self.ktype, self.vtype)).pointer()
            return (data_node.cast(gdb.lookup_type('char').pointer()) - self.payload()).cast(node_type)

        def __next__(self):
            if self.data_node == self.val['e']:
                raise StopIteration
            node = self.concrete(self.data_node).dereference()
            if self.count % 2 == 0:
                item = node['key']
            else:
                item = node['value']
                self.data_node = node['forward'][0]

            result = ('[%d]' % self.count, item)
            self.count = self.count + 1
            return result


    def __init__(self, val, container):
        self.val = val
        self.container = container

    def children(self):
        isQt4 = has_field(self.val, 'e') # Qt4 has 'e', Qt5 doesn't
        if isQt4:
            return self._iterator(self.val)
        else:
            # TODO: Add proper iterator for Qt5-based QMap
            return default_iterator(self.val)

    def to_string(self):
        if self.val['d']['size'] == 0:
            empty = "empty "
        else:
            empty = ""

        return "%s%s<%s, %s>" % ( empty, self.container, self.val.type.template_argument(0), self.val.type.template_argument(1) )

    def display_hint (self):
        return 'map'

class QHashPrinter:
    "Print a QHash"

    class _iterator(Iterator):
        def __init__(self, val):
            self.val = val
            self.d = self.val['d']
            self.ktype = self.val.type.template_argument(0)
            self.vtype = self.val.type.template_argument(1)
            self.end_node = self.d.cast(gdb.lookup_type('QHashData::Node').pointer())
            self.data_node = self.firstNode()
            self.count = 0

        def __iter__(self):
            return self

        def hashNode (self):
            "Casts the current QHashData::Node to a QHashNode and returns the result. See also QHash::concrete()"
            return self.data_node.cast(gdb.lookup_type('QHashNode<%s, %s>' % (self.ktype, self.vtype)).pointer())

        def firstNode (self):
            "Get the first node, See QHashData::firstNode()."
            e = self.d.cast(gdb.lookup_type('QHashData::Node').pointer())
            #print "QHashData::firstNode() e %s" % e
            bucketNum = 0
            bucket = self.d['buckets'][bucketNum]
            #print "QHashData::firstNode() *bucket %s" % bucket
            n = self.d['numBuckets']
            #print "QHashData::firstNode() n %s" % n
            while n:
                #print "QHashData::firstNode() in while, n %s" % n;
                if bucket != e:
                    #print "QHashData::firstNode() in while, return *bucket %s" % bucket
                    return bucket
                bucketNum += 1
                bucket = self.d['buckets'][bucketNum]
                #print "QHashData::firstNode() in while, new bucket %s" % bucket
                n -= 1
            #print "QHashData::firstNode() return e %s" % e
            return e


        def nextNode (self, node):
            "Get the nextNode after the current, see also QHashData::nextNode()."
            #print "******************************** nextNode"
            #print "nextNode: node %s" % node
            next = node['next'].cast(gdb.lookup_type('QHashData::Node').pointer())
            e = next

            #print "nextNode: next %s" % next
            if next['next']:
                #print "nextNode: return next"
                return next

            #print "nextNode: node->h %s" % node['h']
            #print "nextNode: numBuckets %s" % self.d['numBuckets']
            start = (node['h'] % self.d['numBuckets']) + 1
            bucketNum = start
            #print "nextNode: start %s" % start
            bucket = self.d['buckets'][start]
            #print "nextNode: bucket %s" % bucket
            n = self.d['numBuckets'] - start
            #print "nextNode: n %s" % n
            while n:
                #print "nextNode: in while; n %s" % n
                #print "nextNode: in while; e %s" % e
                #print "nextNode: in while; *bucket %s" % bucket
                if bucket != e:
                    #print "nextNode: in while; return bucket %s" % bucket
                    return bucket
                bucketNum += 1
                bucket = self.d['buckets'][bucketNum]
                n -= 1
            #print "nextNode: return e %s" % e
            return e

        def __next__(self):
            "GDB iteration, first call returns key, second value and then jumps to the next hash node."
            if self.data_node == self.end_node:
                raise StopIteration

            node = self.hashNode()

            if self.count % 2 == 0:
                item = node['key']
            else:
                item = node['value']
                self.data_node = self.nextNode(self.data_node)

            self.count = self.count + 1
            return ('[%d]' % self.count, item)

    def __init__(self, val, container):
        self.val = val
        self.container = container

    def children(self):
        return self._iterator(self.val)

    def to_string(self):
        if self.val['d']['size'] == 0:
            empty = "empty "
        else:
            empty = ""

        return "%s%s<%s, %s>" % ( empty, self.container, self.val.type.template_argument(0), self.val.type.template_argument(1) )

    def display_hint (self):
        return 'map'

class QDatePrinter:

    def __init__(self, val):
        self.val = val

    def to_string(self):
        julianDay = self.val['jd']

        if julianDay == 0:
            return "invalid QDate"

        # Copied from Qt sources
        if julianDay >= 2299161:
            # Gregorian calendar starting from October 15, 1582
            # This algorithm is from Henry F. Fliegel and Thomas C. Van Flandern
            ell = julianDay + 68569;
            n = (4 * ell) / 146097;
            ell = ell - (146097 * n + 3) / 4;
            i = (4000 * (ell + 1)) / 1461001;
            ell = ell - (1461 * i) / 4 + 31;
            j = (80 * ell) / 2447;
            d = ell - (2447 * j) / 80;
            ell = j / 11;
            m = j + 2 - (12 * ell);
            y = 100 * (n - 49) + i + ell;
        else:
            # Julian calendar until October 4, 1582
            # Algorithm from Frequently Asked Questions about Calendars by Claus Toendering
            julianDay += 32082;
            dd = (4 * julianDay + 3) / 1461;
            ee = julianDay - (1461 * dd) / 4;
            mm = ((5 * ee) + 2) / 153;
            d = ee - (153 * mm + 2) / 5 + 1;
            m = mm + 3 - 12 * (mm / 10);
            y = dd - 4800 + (mm / 10);
            if y <= 0:
                --y;
        return "%d-%02d-%02d" % (y, m, d)

class QTimePrinter:

    def __init__(self, val):
        self.val = val

    def to_string(self):
        ds = self.val['mds']

        if ds == -1:
            return "invalid QTime"

        MSECS_PER_HOUR = 3600000
        SECS_PER_MIN = 60
        MSECS_PER_MIN = 60000

        hour = ds / MSECS_PER_HOUR
        minute = (ds % MSECS_PER_HOUR) / MSECS_PER_MIN
        second = (ds / 1000)%SECS_PER_MIN
        msec = ds % 1000
        return "%02d:%02d:%02d.%03d" % (hour, minute, second, msec)

class QDateTimePrinter:

    def __init__(self, val):
        self.val = val

    def to_string(self):
        #val['d'] is a QDateTimePrivate, but for some reason casting to that doesn't work
        #so work around by manually adjusting the pointer
        date = self.val['d'].cast(gdb.lookup_type('char').pointer());
        date += gdb.lookup_type('int').sizeof #increment for QAtomicInt ref;
        date = date.cast(gdb.lookup_type('QDate').pointer()).dereference();

        time = self.val['d'].cast(gdb.lookup_type('char').pointer());
        time += gdb.lookup_type('int').sizeof + gdb.lookup_type('QDate').sizeof #increment for QAtomicInt ref; and QDate date;
        time = time.cast(gdb.lookup_type('QTime').pointer()).dereference();
        return "%s %s" % (date, time)

class QUrlPrinter:

    def __init__(self, val):
        self.val = val

    def to_string(self):
        try:
            return self.val['d']['encodedOriginal']
        except RuntimeError:
            #if no debug information is avaliable for Qt, try guessing the correct address for encodedOriginal
            #problem with this is that if QUrlPrivate members get changed, this fails
            offset = gdb.lookup_type('int').sizeof
            offset += offset % gdb.lookup_type('void').pointer().sizeof #alignment
            offset += gdb.lookup_type('QString').sizeof * 6
            offset += gdb.lookup_type('QByteArray').sizeof
            encodedOriginal = self.val['d'].cast(gdb.lookup_type('char').pointer());
            encodedOriginal += offset
            encodedOriginal = encodedOriginal.cast(gdb.lookup_type('QByteArray').pointer()).dereference();
            encodedOriginal = encodedOriginal['d']['data'].string()
            return encodedOriginal

class QSetPrinter:
    "Print a QSet"

    def __init__(self, val):
        self.val = val

    class _iterator(Iterator):
        def __init__(self, hashIterator):
            self.hashIterator = hashIterator
            self.count = 0

        def __iter__(self):
            return self

        def __next__(self):
            if self.hashIterator.data_node == self.hashIterator.end_node:
                raise StopIteration

            node = self.hashIterator.hashNode()

            item = node['key']
            self.hashIterator.data_node = self.hashIterator.nextNode(self.hashIterator.data_node)

            self.count = self.count + 1
            return ('[%d]' % (self.count-1), item)

    def children(self):
        hashPrinter = QHashPrinter(self.val['q_hash'], None)
        hashIterator = hashPrinter._iterator(self.val['q_hash'])
        return self._iterator(hashIterator)

    def to_string(self):
        if self.val['q_hash']['d']['size'] == 0:
            empty = "empty "
        else:
            empty = ""

        return "%sQSet<%s>" % ( empty , self.val.type.template_argument(0) )


class QCharPrinter:

    def __init__(self, val):
        self.val = val

    def to_string(self):
        return unichr(self.val['ucs'])

    def display_hint (self):
        return 'string'

class QUuidPrinter:

    def __init__(self, val):
        self.val = val

    def to_string(self):
        return "QUuid({%x-%x-%x-%x%x-%x%x%x%x%x%x})" % (self.val['data1'], self.val['data2'], self.val['data3'],
                                            self.val['data4'][0], self.val['data4'][1],
                                            self.val['data4'][2], self.val['data4'][3],
                                            self.val['data4'][4], self.val['data4'][5],
                                            self.val['data4'][6], self.val['data4'][7])

    def display_hint (self):
        return 'string'

pretty_printers_dict = {}

def register_qt4_printers (obj):
    if obj == None:
        obj = gdb

    obj.pretty_printers.append(FunctionLookup(gdb, pretty_printers_dict))

def build_dictionary ():
    pretty_printers_dict[re.compile('^QString$')] = lambda val: QStringPrinter(val)
    pretty_printers_dict[re.compile('^QByteArray$')] = lambda val: QByteArrayPrinter(val)
    pretty_printers_dict[re.compile('^QList<.*>$')] = lambda val: QListPrinter(val, 'QList', None)
    pretty_printers_dict[re.compile('^QStringList$')] = lambda val: QListPrinter(val, 'QStringList', 'QString')
    pretty_printers_dict[re.compile('^QQueue')] = lambda val: QListPrinter(val, 'QQueue', None)
    pretty_printers_dict[re.compile('^QVector<.*>$')] = lambda val: QVectorPrinter(val, 'QVector')
    pretty_printers_dict[re.compile('^QStack<.*>$')] = lambda val: QVectorPrinter(val, 'QStack')
    pretty_printers_dict[re.compile('^QLinkedList<.*>$')] = lambda val: QLinkedListPrinter(val)
    pretty_printers_dict[re.compile('^QMap<.*>$')] = lambda val: QMapPrinter(val, 'QMap')
    pretty_printers_dict[re.compile('^QMultiMap<.*>$')] = lambda val: QMapPrinter(val, 'QMultiMap')
    pretty_printers_dict[re.compile('^QHash<.*>$')] = lambda val: QHashPrinter(val, 'QHash')
    pretty_printers_dict[re.compile('^QMultiHash<.*>$')] = lambda val: QHashPrinter(val, 'QMultiHash')
    pretty_printers_dict[re.compile('^QDate$')] = lambda val: QDatePrinter(val)
    pretty_printers_dict[re.compile('^QTime$')] = lambda val: QTimePrinter(val)
    pretty_printers_dict[re.compile('^QDateTime$')] = lambda val: QDateTimePrinter(val)
    pretty_printers_dict[re.compile('^QUrl$')] = lambda val: QUrlPrinter(val)
    pretty_printers_dict[re.compile('^QSet<.*>$')] = lambda val: QSetPrinter(val)
    pretty_printers_dict[re.compile('^QChar$')] = lambda val: QCharPrinter(val)
    pretty_printers_dict[re.compile('^QUuid')] = lambda val: QUuidPrinter(val)


build_dictionary ()
