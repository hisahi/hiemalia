
s 1*plain

# flying 1
x 6 0 0 0 setspeed fast

o 3 0 0.5 0.125 box 1 0.5 0.0625
o 3 0 -0.5 0.125 box 1 0.5 0.0625
o 3 0 0 0.125 box 1 0.5 0.0625

od 3
o 3 0.5 0 0.125 box 0.5 1 0.0625
o 3 -0.5 0 0.125 box 0.5 1 0.0625
o 3 0 0 0.125 box 0.375 1 0.0625

od 4
# shooting 1
x 1.25 0 0 0 checkpoint
x 1.5 0 0 0 setspeed slow

o 0.5 -0.5 -0.25 0.25 shard
o 0.5 -0.25 -0.125 0.25 shard
o 0.5 0 0 0.25 shard
o 0.5 0.25 0.125 0.25 shard
o 0.5 0.5 0.25 0.25 shard

od 1.5
o 0.5 0.5 -0.25 0.25 shard
o 0.5 0.25 -0.125 0.25 shard
o 0.5 0 0 0.25 shard
o 0.5 -0.25 0.125 0.25 shard
o 0.5 -0.5 0.25 0.25 shard

od 1
o 0 -0.5 0 0 gunboat
o 0 0.5 0 0 gunboat

od 1.5
o 0 0 -0.5 0 gunboat
o 0 0 0.5 0 gunboat

od 2.5
o 0 0 0 0 box 1 0.25 0.0625
or 1 0 -0.625 0 gunboat
or 1 0 0.625 0 gunboat
od 2.5

od 2.5
o 0 -0.75 0 0 box 0.25 1 0.0625
o 0 0.75 0 0 box 0.25 1 0.0625
or 2 -0.5 0 0 gunboat
or 2 0.5 0 0 gunboat

od 5
# flying 2
x 0 0 0 0 checkpoint
x 0.25 0 0 0 setspeed fast

od 3
o 0 -0.75 0 0 box 0.25 1 0.0625
o 0 0.75 0 0 box 0.25 1 0.0625

od 3
o 0 0 0 0 box 0.5 1 0.0625

od 3
o 0 0 0.625 0 box 1 0.375 0.0625
o 0 0 -0.625 0 box 1 0.375 0.0625

od 3
o 0 0 0.75 0 box 1 0.25 0.0625
o 0 0 -0.5 0 box 1 0.5 0.0625

od 3
o 0 0 0.625 0 box 1 0.375 0.0625
o 0 0 -0.625 0 box 1 0.375 0.0625

od 3
o 0 0 0.5 0 box 1 0.5 0.0625
o 0 0 -0.75 0 box 1 0.25 0.0625

od 4
# shooting 3
x 0.25 0 0 0 checkpoint
x 1.5 0 0 0 setspeed slow

o 0 -0.5 0 0 shard
o 0 0.5 0 0 shard
o 0 0 -0.5 0 shard
o 0 0 0.5 0 shard

od 2
o 0 -0.5 -0.5 0 gunboat
o 0 -0.5 0.5 0 gunboat
o 0 0.5 -0.5 0 gunboat
o 0 0.5 0.5 0 gunboat

od 5
o 5 0 0 0 chevron

o 2 0 0 0 chevron
o 0 -0.5 -0.5 0 shard
o 0 -0.5 0.5 0 shard
o 0 0.5 -0.5 0 shard
o 0 0.5 0.5 0 shard

od 3
o 0 0 -0.5 0 gunboat
o 0 0 0.5 0 gunboat

od 0.5
o 0 -0.25 0 0 chevron
o 0 0.25 0 0 chevron

od 1
o 0 -0.5 -0.5 0 box 0.5 0.5 0.0625
o 0 0.5 0.5 0 box 0.5 0.5 0.0625

od 1.5
o 0 0 0 0 volcano 0 0 0 0.1 0 0

od 3
o 0 -0.5 -0.5 0 box 0.5 0.5 0.0625
o 0 0.5 0.5 0 box 0.5 0.5 0.0625

od 2
o 1.5 0 0 0.5 box 0.0625 1 1
or 2 -0.5 -0.375 0 chevron
or 3.5 -0.5 0.375 0 chevron
or 5 -0.5 -0.375 0 chevron
or 7 -0.5 0.375 0 chevron
or 7 -0.5 -0.375 0 chevron
or 2.5 0.5 -0.5 0 volcano 0 0.05 0 0 0.25 0
or 4 0.5 0.5 0 volcano 0 -0.05 0 0 -0.25 0
or 5.5 0.5 -0.5 0 volcano 0 0.05 0 0 0.25 0
or 7 0.5 0.5 0 volcano 0 -0.05 0 0 -0.25 0
o 2 0 0 0.5 box 0.0625 1 1
o 2 0 0 0.5 box 0.0625 1 1
o 2 0 0 0.5 box 0.0625 1 1
o 1 0 0 0.5 box 0.0625 1 1
o 4.5 -0.5 -0.5 0.375 box 0.5 0.5 0.0625

o 0 0 -0.75 0 box 1 0.25 0.0625
o 1.5 0 0.75 0 box 1 0.25 0.0625

o 0 0 -0.75 0 volcano 0 1.25 0 0 0.25 0
o 2.5 0 0.75 0 volcano 0 -1.25 0 0 -0.25 0

# flying 3
x 1.25 0 0 0 checkpoint
x 0.25 0 0 0 setspeed fast

od 2
o 0 -0.6666666 -0.6666666 0 box 0.3333333 0.3333333 0.0625
o 0 -0.6666666 0 0 box 0.3333333 0.3333333 0.0625
o 0 -0.6666666 0.6666666 0 box 0.3333333 0.3333333 0.0625
o 0 0 -0.6666666 0 box 0.3333333 0.3333333 0.0625
o 0 0 0.6666666 0 box 0.3333333 0.3333333 0.0625
o 0 0.6666666 -0.6666666 0 box 0.3333333 0.3333333 0.0625
o 0 0.6666666 0 0 box 0.3333333 0.3333333 0.0625
o 0 0.6666666 0.6666666 0 box 0.3333333 0.3333333 0.0625

od 2.5
o 0 0.5 -0.5 0 box 0.5 0.5 0.0625
o 0 -0.5 0.5 0 box 0.5 0.5 0.0625

od 2.5
o 0 -0.75 -0.75 0 box 0.25 0.25 0.0625
o 0 0.25 -0.75 0 box 0.25 0.25 0.0625
o 0 -0.25 -0.25 0 box 0.25 0.25 0.0625
o 0 0.75 -0.25 0 box 0.25 0.25 0.0625
o 0 -0.75 0.25 0 box 0.25 0.25 0.0625
o 0 0.25 0.25 0 box 0.25 0.25 0.0625
o 0 -0.25 0.75 0 box 0.25 0.25 0.0625
o 0 0.75 0.75 0 box 0.25 0.25 0.0625

od 2.5
o 0 -0.75 0 0 box 0.25 0.5 0.0625
o 0 -0.25 0.25 0 box 0.25 0.75 0.0625
o 0 0.25 -0.25 0 box 0.25 0.75 0.0625
o 0 0.75 0 0 box 0.25 0.5 0.0625

od 4
x 0 0 0 0 setspeed slow
or 3 0 0 0 boss0
od 1

od 30

o 20 0 0 0 boss1
xr 0.1 0 0 1 stageend
