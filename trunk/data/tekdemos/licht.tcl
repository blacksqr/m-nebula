#
#   Std-Lights, used by most of the demos
#
#   upd.:   30-Jul-99
#
#   (C) 1999 RADON LABS   Beyreuther, Weissflog, Flemming GbR
#   (C) COPYRIGHT 2000 RADONLABS GMBH
#

sel /usr/scene

new n3dnode dlight
	sel dlight

	new n3dnode light1
		sel light1
		new nlightnode light
			sel light
		    .setattenuation 1 0 0 
			.settype  point
			.setcolor 0.1 0.5 1 1
		sel ..	
		.txyz -50 5 50
	sel ..

	new n3dnode light2
		sel light2
		new nlightnode light
			sel light
			.setattenuation 1 0 0 
			.settype  point
			.setcolor 1 0.5 0.1 1
		sel ..	
		.txyz 50 5 50
	sel ..

    new nlightnode amb
        sel amb
		.settype  ambient
        .setcolor 0.2 0.2 0.2 0
    sel ..

sel ..
