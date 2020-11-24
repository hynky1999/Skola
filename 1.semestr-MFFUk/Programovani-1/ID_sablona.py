class Prvek:
    def __init__(self, x, dalsi):
        self.x = x
        self.dalsi = dalsi

def VytiskniLSS( p ):
    print( "LSS:", end=" " )
    while p!=None:
        print( p.x, end=" " )
        p = p.dalsi
    print(".")

def NactiLSS():
    """cte cisla z radku, dokud nenacte prazdny radek"""
    prvni = None
    posledni = None
    r = input()
    while r!="":
        radek = r.split()
        if len(radek)==0: # protoze ten test r!="" v RCDX neukoncil cyklus!
            break
        for s in radek:
            p = Prvek(int(s),None)
            if prvni==None:
                prvni = p
            else:
                posledni.dalsi = p
            posledni = p
        r = input()
    return prvni

#################################################

def IntersectionDestruct(a,b):
    """ destruktivni prunik dvou usporadanych seznamu
    * nevytvari zadne nove prvky, vysledny seznam bude poskladany z prvku puvodnich seznamu,
    * vysledek je MNOZINA, takze se hodnoty neopakuji """
    head, last = [None]*2
    # a and b are check
    while(a and b):
        if(a.x < b.x):
            a = a.dalsi
        elif(a.x > b.x):
            b = b.dalsi
        else:
            if last:
                last.dalsi = a
                last = last.dalsi

            else:
                head = a
                last = head

            val = a.x
            while(a):
                if(a.x != val):
                    break
                a = a.dalsi

            while(b):
                if(b.x != val):
                    break
                b = b.dalsi
    if(last):
        last.dalsi = None
    return head




    # sem doplnte kod funkce, dalsi casti zdrojoveho kodu NEMENTE
    # ....................................................
    # ....................................................
    # ....................................................
    # ....................................................
    # ....................................................
    # ....................................................
    # ....................................................
    # ....................................................
    # ....................................................

#################################################

VytiskniLSS( IntersectionDestruct( NactiLSS(), NactiLSS() ) )
