convert font.png -filter Jinc \( +clone -negate -morphology Distance Euclidean -level 50%,-50% \) -morphology Distance Euclidean -compose Plus -composite -level 43%,57% -resize 12.5% out-font.png
