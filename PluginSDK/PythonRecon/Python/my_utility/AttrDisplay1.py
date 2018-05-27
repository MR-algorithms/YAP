class ListInherited:
    """if __repr__ insteaded of __str__, it will happen recursion
    For 'attr' in for-circle is string, it will call __repr__ again"""
    def __str__(self):
        return '<Class: %s instance, Address %s:\n %s>' % (self.__class__.__name__, id(self), self.__attrnames())

    def __attrnames(self):
        result = ''
        for attr in sorted(dir(self)):
            if attr[:2] == '__' and attr[-2:] == '__':
                result += '\tname %s = <>\n' % attr
            else:
                result += '\tname %s = %s\n' % (attr,getattr(self, attr))

        return result



if __name__== '__main__':
    class Super:
        def __init__(self):
            self.data1 = 'super data1'
        def ham(self):
            pass
        def getIt(name):
            print(name)

    class Sub(Super, ListInherited):
        def __init__(self):
            Super.__init__(self)
            self.data2 = 'sub data2'
            self.data3 = 42

        def spam(self):
            pass


    x = Sub()
    print(x)
