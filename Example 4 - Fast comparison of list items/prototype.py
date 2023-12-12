import math
#times optimised() and standard()
def main():
    import time

    t0 = time.time() #start timer
    
    standard() #replace with optimised() or optimised()
    
    t1 = time.time() #end timer

    total = t1-t0
    print(total)




#nested for loop and conditional check
def standard():
    len = 20
    for x in range(len):
        for y in range(len):
            if(x!=y):
                print(x)
                print(y)



#single for loop gets x and y elements in an array once without x==y
def optimised():
    len = 20
    enumerations = int(len*(len-1)/2)
    for i in range(enumerations):
        #indexA solves quadratic equation as n*(n-1) /2 = indexA
        indexA = int((-1 + math.sqrt(1 + 8 * i)) / 2) + 1
        indexB = i % indexA if indexA != 1 else 0
        print(indexA)
        print(indexB)
        
if __name__ == "__main__":
    main()
