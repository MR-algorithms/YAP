# SubTest how to get the attribution attr1 & attr2 ???
# ( instance y can get x attribution)
# Is superclass's instance inherited to the subclass's instance ?

class AttrDisplay:

    def __init__(self):
        self.name = 'base class'
        
    def gatherAttrs(self):
        attr = []
        for key in sorted(self.__dict__):
            attr.append('%s=%s'% (key,getattr(self,key)))
        return ','.join(attr)

    def __str__(self):
        return '[%s : %s]' %(self.__class__.__name__, self.gatherAttrs())


if __name__ == '__main__':
    class TopTest(AttrDisplay):
        count = 0
        def __init__(self):
            self.lastname = AttrDisplay().name
            self.attr1 = TopTest.count
            self.attr2 = TopTest.count + 1
            TopTest.count += 2


    class SubTest(TopTest):
        pass


    x,y = TopTest(), SubTest()
    print(x)
    print(y)
    
