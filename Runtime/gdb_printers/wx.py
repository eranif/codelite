# Pretty-printers for wxWidgets.
import gdb
import itertools
import re
import platform

class wxStringPrinter:

    def __init__(self, val):
        self.val = val

    def to_string(self):
        ret = ""
        wx29 = 0
        try:
            # wx-28 has m_pchData
            self.val['m_pchData']
        except Exception:
            wx29 = 1

        try:
            if wx29:
                dataAsCharPointer = '"' + self.val['m_impl']['_M_dataplus']['_M_p'].string() + '"'
            else:
                dataAsCharPointer = '"' + self.val['m_pchData'].string() + '"'
            ret = dataAsCharPointer
        except Exception:
            # swallow the exception and return empty string
            pass
        return ret

    def display_hint (self):
        return 'wxString'
        
class wxArrayFooPrinter:
    "For wxArrayString, wxArrayInt, wxArrayDouble etc"

    class _iterator:
        def __init__ (self, items, item_count):
            self.items = items
            self.item_count = item_count
            self.count = 0

        def __iter__(self):
            return self

        def next(self):
            count = self.count
            self.count = self.count + 1
            if count >= self.item_count or count > 5000: # Even 5000 gives a noticeable pause
                raise StopIteration
            try:
                # Try the wx >=2.9 way first
                elt = self.items[count]['m_impl']
            except Exception:
                # The wx2.8 way
                elt = self.items[count]
            return ('[%d]' % count, elt)

         # Python3 version
        def __next__(self):
            return self.next()

    def __init__(self, val):
        self.val = val

    def children(self):
        return self._iterator(self.val['m_pItems'], self.val['m_nCount'])

    def to_string(self):
        # Ideal would be to return e.g. "wxArrayInt", but how to find the type?
        return "wxArray<T>"


    def display_hint(self):
        return 'array'


class wxFileNamePrinter:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        # It is simpler to just call the internal function here than to iterate
        # over m_dirs array ourselves. The disadvantage of this approach is
        # that it requires a live inferior process and so doesn't work when
        # debugging using only a core file. If this ever becomes a serious
        # problem, this should be rewritten to use m_dirs and m_name and m_ext.
        return gdb.parse_and_eval('((wxFileName*)%s)->GetFullPath(0)' %
                                  self.val.address)     


class wxXYPrinterBase:
    def __init__(self, val):
        self.x = val['x']
        self.y = val['y']

class wxPointPrinter(wxXYPrinterBase):
    def to_string(self):
        return '(%d, %d)' % (self.x, self.y)

class wxSizePrinter(wxXYPrinterBase):
    def to_string(self):
        return '%d*%d' % (self.x, self.y)

class wxRectPrinter(wxXYPrinterBase):
    def __init__(self, val):
        wxXYPrinterBase.__init__(self, val)
        self.width = val['width']
        self.height = val['height']

    def to_string(self):
        return '(%d, %d) %d*%d' % (self.x, self.y, self.width, self.height)

def register_wx_printers (obj):
    if obj == None:
        obj = gdb

    obj.pretty_printers.append (lookup_function)

def lookup_function (val):
    "Look-up and return a pretty-printer that can print val."

    # Get the type.
    type = val.type;

    # If it points to a reference, get the reference.
    if type.code == gdb.TYPE_CODE_REF:
        type = type.target ()

    # Get the unqualified type, stripped of typedefs.
    type = type.unqualified ().strip_typedefs ()

    # Get the type name.
    typename = type.tag
    if typename == None:
        return None

    # Iterate over local dictionary of types to determine
    # if a printer is registered for that type.  Return an
    # instantiation of the printer if found.
    for function in pretty_printers_dict:
        if function.search (typename):
            return pretty_printers_dict[function] (val)

    # Cannot find a pretty printer.  Return None.
    return None

def build_dictionary ():
    pretty_printers_dict[re.compile('^wxString$')] = lambda val: wxStringPrinter(val)
    pretty_printers_dict[re.compile('^wxArray.+$')] = lambda val: wxArrayFooPrinter(val)
    if platform.system() != 'Windows':
        pretty_printers_dict[re.compile('^wxFileName$')] = lambda val: wxFileNamePrinter(val)
    pretty_printers_dict[re.compile('^wxPoint$')] = lambda val: wxPointPrinter(val)
    pretty_printers_dict[re.compile('^wxSize$')] = lambda val: wxSizePrinter(val)
    pretty_printers_dict[re.compile('^wxRect$')] = lambda val: wxRectPrinter(val)

pretty_printers_dict = {}
build_dictionary ()
