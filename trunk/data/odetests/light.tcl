#-- setup some default lights
sel /lights
  new n3dnode dlight
  sel dlight
  
  new n3dnode light1
  sel light1
    new nlightnode light
    sel light
      setattenuation 1 0 0
      settype directional
      setcolor 1 1 1 1
      setcastshadows true
    sel ..
    rx -45
    txyz 0 200 30
  sel ..
  
  new nlightnode amb
  sel amb
    settype ambient
    setcolor 0.2 0.2 0.2 0
  sel ..
sel ..