#-- map input

sel /sys/servers/input
  .beginmap
    .map keyb0:f8.down     		"script:/sys/servers/console.watch *"
    .map keyb0:f7.up     		"script:/sys/servers/console.unwatch"
    .map keyb0:up.pressed  		cam_forward
    .map keyb0:down.pressed  	cam_back
	.map keyb0:left.pressed   	cam_left
	.map keyb0:right.pressed   	cam_right	
    .map keyb0:u.pressed   		cam_up
    .map keyb0:j.pressed   		cam_down    
    .map relmouse0:+y    		cam_look_up
    .map relmouse0:-y    		cam_look_down
    .map relmouse0:-x    		cam_turn_left
    .map relmouse0:+x    		cam_turn_right
    .map keyb0:w.pressed   		speed_up
    .map keyb0:s.pressed   		slow_down
    .map keyb0:a.pressed   		steer_left
    .map keyb0:d.pressed   		steer_right
    .map keyb0:space.pressed 	resetbuggy
    .map keyb0:e.pressed   		hinge_error
    .map keyb0:1.pressed   		test_hinge
    .map keyb0:2.pressed   		test_buggy
    .map keyb0:3.pressed   		test_composite
    .map keyb0:4.pressed   		test_mesh
    .map keyb0:8.pressed   		sel_sphere
    .map keyb0:9.pressed   		sel_capsule
    .map keyb0:0.pressed   		sel_box
    .map keyb0:esc.down    		"script:/main.stop"
    .map keyb0:f11.down    		"script:/sys/servers/console.toggle"
  .endmap
sel /