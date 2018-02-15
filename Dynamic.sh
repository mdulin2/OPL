#!/bin/bash
print_something(){
	compare="hump"
	if [ "$my_hump" = "$compare" ]; then
		echo $my_hump
		print_something3
	else
		print_something2
	fi
}

print_something2(){
	echo In print_something2
	echo $my_hump
}

print_something3(){
	echo In print_something3
	my_hump="hump day!"
	print_something2

}
func_call(){
	my_hump="hump"
	print_something $my_hump
}

func_call
print_something2 "t"
#dynamic scope is very interesting here!
#Even though in print_something3 the variable my_hump is not declared,
#it is still able to call it because it was declared in a function above it.
#In another interesting turn, the function above it doesn't even need
# to call it. If i run func_call, then run print_something3 afterwards,
# then the print for hump still exists!
