# Pretty-printers for wxWidgets.
import gdb
import itertools
import re

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
                dataAsCharPointer = self.val['m_impl']['_M_dataplus']['_M_p']
            else:
                dataAsCharPointer = self.val['m_pchData']
            ret = dataAsCharPointer
        except Exception:
            # swallow the exception and return empty string
            pass
        return ret

    def display_hint (self):
        return 'wxString'

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

pretty_printers_dict = {}
build_dictionary ()
