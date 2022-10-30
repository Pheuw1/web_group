#!/usr/bin/python
import sys

def safe(l,n):
	for i in range(n+1):
		for j in range(n+1):
			if (i != j):	
				if (abs(l[i] - l[j])  == abs(i-j) or l[i] == l[j]):
					return 0
	return 1

def solve(l,n,c,q):
	i=0
	while(i < q and n < q):
		l[n] = i
		if (safe(l,n)): 
			if (n == q - 1): c += 1
			c = solve(l,n+1,c,q)
		i+=1
	return c

if __name__ == "__main__":
	if (len(sys.argv) > 1):
		l = [0] * (int(sys.argv[1]))
		print(solve(l,0,0,int(sys.argv[1])))
	else:
		print ("need one arg: the number of queens")
		print ("solving for default: 8 queens")
		print("\n")
		l = [0] * (8)
		print("Result : ",solve(l,0,0,8))	
	exit()
