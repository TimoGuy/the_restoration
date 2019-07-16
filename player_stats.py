

"""
Output for player_mvt_stats(0, -5.5, 1, 0.5, 35)

step=0,hsp=0,vsp=0,coords=(0,0)
step=1,hsp=1,vsp=-5.5,coords=(1,-5.5)
step=2,hsp=2,vsp=-5.0,coords=(3,-10.5)
step=3,hsp=3,vsp=-4.5,coords=(6,-15.0)
step=4,hsp=4,vsp=-4.0,coords=(10,-19.0)
step=5,hsp=5,vsp=-3.5,coords=(15,-22.5)
step=6,hsp=6,vsp=-3.0,coords=(21,-25.5)
step=7,hsp=7,vsp=-2.5,coords=(28,-28.0)
step=8,hsp=8,vsp=-2.0,coords=(36,-30.0)
step=9,hsp=9,vsp=-1.5,coords=(45,-31.5)
step=10,hsp=10,vsp=-1.0,coords=(55,-32.5)
step=11,hsp=11,vsp=-0.5,coords=(66,-33.0)
step=12,hsp=12,vsp=0.0,coords=(78,-33.0)			-----CREST: 66-78px, which is just 2 blocks wide (64px)
step=13,hsp=13,vsp=0.5,coords=(91,-32.5)
step=14,hsp=14,vsp=1.0,coords=(105,-31.5)
step=15,hsp=15,vsp=1.5,coords=(120,-30.0)
step=16,hsp=16,vsp=2.0,coords=(136,-28.0)
step=17,hsp=17,vsp=2.5,coords=(153,-25.5)
step=18,hsp=18,vsp=3.0,coords=(171,-22.5)
step=19,hsp=19,vsp=3.5,coords=(190,-19.0)
step=20,hsp=20,vsp=4.0,coords=(210,-15.0)
step=21,hsp=21,vsp=4.5,coords=(231,-10.5)
step=22,hsp=22,vsp=5.0,coords=(253,-5.5)
step=23,hsp=23,vsp=5.5,coords=(276,0.0)				-----TOUCHDOWN: 276px, which is (256px) 8 blocks wide
step=24,hsp=24,vsp=6.0,coords=(300,6.0)
step=25,hsp=25,vsp=6.5,coords=(325,12.5)
step=26,hsp=26,vsp=7.0,coords=(351,19.5)
step=27,hsp=27,vsp=7.5,coords=(378,27.0)
step=28,hsp=28,vsp=8.0,coords=(406,35.0)
step=29,hsp=29,vsp=8.5,coords=(435,43.5)
step=30,hsp=30,vsp=9.0,coords=(465,52.5)
step=31,hsp=31,vsp=9.5,coords=(496,62.0)
step=32,hsp=32,vsp=10.0,coords=(528,72.0)
step=33,hsp=33,vsp=10.5,coords=(561,82.5)
step=34,hsp=34,vsp=11.0,coords=(595,93.5)
step=35,hsp=35,vsp=11.5,coords=(630,105.0)








Output for player_mvt_stats(35, -5.5, 0, 0.5, 35)

step=0,hsp=0,vsp=0,coords=(0,0)
step=1,hsp=35,vsp=-5.5,coords=(35,-5.5)
step=2,hsp=35,vsp=-5.0,coords=(70,-10.5)
step=3,hsp=35,vsp=-4.5,coords=(105,-15.0)
step=4,hsp=35,vsp=-4.0,coords=(140,-19.0)
step=5,hsp=35,vsp=-3.5,coords=(175,-22.5)
step=6,hsp=35,vsp=-3.0,coords=(210,-25.5)
step=7,hsp=35,vsp=-2.5,coords=(245,-28.0)
step=8,hsp=35,vsp=-2.0,coords=(280,-30.0)
step=9,hsp=35,vsp=-1.5,coords=(315,-31.5)
step=10,hsp=35,vsp=-1.0,coords=(350,-32.5)
step=11,hsp=35,vsp=-0.5,coords=(385,-33.0)
step=12,hsp=35,vsp=0.0,coords=(420,-33.0)			-----CREST: 385-420px, which is 12-13 blocks wide (384-416px)
step=13,hsp=35,vsp=0.5,coords=(455,-32.5)
step=14,hsp=35,vsp=1.0,coords=(490,-31.5)
step=15,hsp=35,vsp=1.5,coords=(525,-30.0)
step=16,hsp=35,vsp=2.0,coords=(560,-28.0)
step=17,hsp=35,vsp=2.5,coords=(595,-25.5)
step=18,hsp=35,vsp=3.0,coords=(630,-22.5)
step=19,hsp=35,vsp=3.5,coords=(665,-19.0)
step=20,hsp=35,vsp=4.0,coords=(700,-15.0)
step=21,hsp=35,vsp=4.5,coords=(735,-10.5)
step=22,hsp=35,vsp=5.0,coords=(770,-5.5)
step=23,hsp=35,vsp=5.5,coords=(805,0.0)				-----TOUCHDOWN: 805px, which is 25 blocks wide (800px)
step=24,hsp=35,vsp=6.0,coords=(840,6.0)
step=25,hsp=35,vsp=6.5,coords=(875,12.5)
step=26,hsp=35,vsp=7.0,coords=(910,19.5)
step=27,hsp=35,vsp=7.5,coords=(945,27.0)
step=28,hsp=35,vsp=8.0,coords=(980,35.0)
step=29,hsp=35,vsp=8.5,coords=(1015,43.5)
step=30,hsp=35,vsp=9.0,coords=(1050,52.5)
step=31,hsp=35,vsp=9.5,coords=(1085,62.0)
step=32,hsp=35,vsp=10.0,coords=(1120,72.0)
step=33,hsp=35,vsp=10.5,coords=(1155,82.5)
step=34,hsp=35,vsp=11.0,coords=(1190,93.5)
step=35,hsp=35,vsp=11.5,coords=(1225,105.0)



"""


def player_mvt_stats(start_hsp, start_vsp, accel_hsp, grav, total_steps):
	steps = 0
	hsp = start_hsp
	vsp = start_vsp
	x = y = 0

	print(f'step=0,hsp=0,vsp=0,coords=(0,0)')
	while steps <= total_steps:
		steps += 1
		print(f'step={steps},hsp={hsp},vsp={vsp},coords=({x},{y})')

		hsp += accel_hsp
		vsp += grav
		x += hsp
		y += vsp






if __name__ == '__main__':
	player_mvt_stats()
