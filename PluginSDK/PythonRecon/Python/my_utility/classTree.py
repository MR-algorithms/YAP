"""
Check Class or object class-tree and show in screen
"""

import types


def subtree(classes, tab):
    print('.' * tab + classes.__name__)
    for super in classes.__bases__:
        subtree(super,tab + 3)

def tree(object):
    if type(object) == type(int): # check is not class type
        object = object()
    
    print('Tree of %s' % object)
    subtree(object.__class__,3)


if __name__ == '__main__':
    class A:        pass
    class B(A):     pass
    class C(A):     pass
    class D(B,C):   pass
    class E:        pass
    class F(D,E):   pass
    tree(B())
    tree(F())
