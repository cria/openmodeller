rm map.tif climproj.tif elevproj.tif
python $1
xview map.tif &
xview climproj.tif &
xview elevproj.tif &
