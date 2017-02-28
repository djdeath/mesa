import parsley
import sys
import re
import warnings

class ConvBinExpr(object):
    def __init__(self, op, left, right):
        self.op = op
        self.left = left
        self.right = right

    def needs_paren(self):
        if len(self.op) > 2:
            return False
        return True

    def to_str(self):
        if len(self.op) > 2:
            return "%s(%s, %s)" % (self.op, self.left.to_str(), self.right.to_str())
        else:
            if self.left.needs_paren():
                left = "(%s)" % self.left.to_str()
            else:
                left = self.left.to_str()

            if self.right.needs_paren():
                right = "(%s)" % self.right.to_str()
            else:
                right = self.right.to_str()

            return "%s %s %s" % (left, self.op, right)


class ConvNameRef(object):
    def __init__(self, name):
        self.name = name

    def needs_paren(self):
        return False

    def to_str(self):
        return self.name

conv_grammar = parsley.makeGrammar("""

top  = expr:e ws end                 -> e

expr = op:o primExpr:r primExpr:l    -> ConvBinExpr(o, l, r)
     | primExpr:e                    -> e

primExpr = expr
         | ident:n                   -> ConvNameRef(n)
         | name:n                    -> ConvNameRef(n)
         | number:n                  -> ConvNameRef(n)

ident       = ws < '$' name_char+ >:s              -> s
name        = ws < name_char+ >:s                  -> s
name_char   = :c ?(re.match('[_a-zA-Z0-9]', c))    -> c

number      = ws ( < '0x' hexa_char+ >:n
                 | < '0b' bin_char+ >:n
                 | < number_char+ >:n )
number_char = :c ?(re.match('[0-9]', c))           -> c
hexa_char   = :c ?(re.match('[A-F0-9]', c))        -> c
bin_char    = :c ?(re.match('[01]', c))            -> c

op   = ws
     ( 'FADD' -> '+'
     | 'FDIV' -> '/'
     | 'FMAX' -> 'max'
     | 'FMUL' -> '*'
     | 'FSUB' -> '-'
     | 'READ' -> 'read'
     | 'UADD' -> '+'
     | 'UDIV' -> '/'
     | 'UMUL' -> '*'
     | 'USUB' -> '-'
     | 'UMIN' -> 'min'
     | 'AND'  -> '&'
     | 'UGTE' -> '>='
     | 'ULT'  -> '<'
     | '&&'   -> '&&' )

ws   = (:c ?(re.match('[\\n\\t ]', c)))*

""", globals())


for line in sys.stdin.readlines():
    attributes = [ 'equation', 'max_equation', 'availability' ]
    replaced = False
    for a in attributes:
        m = re.match("(\s+)%s=\"([^\(\";]+)\"" % a, line)
        if m:
            spaces = line[m.start(1):m.end(1)]
            texpr = line[m.start(2):m.end(2)]
            lexpr = texpr.split(' ')
            lexpr.reverse()
            try:
                new_expr = conv_grammar(' '.join(lexpr)).top().to_str()
                print("%s%s=\"%s\"" % (spaces, a, new_expr))
                replaced = True
                break
            except:
                warnings.warn("Failed to parse \"%s\"" % texpr)
                pass
    if not replaced:
        print(line[0:len(line) - 1])
