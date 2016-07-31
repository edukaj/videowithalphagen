# Usage

This program allow to convert a sequenze of images into a video
The images must have the following signature name_xxx where xxx
is a progressive number starting from one


	Usage:
		videowithalphagen -p image -o video.avi
	Options:
		-h [ --help ]                 produce this message
		-p [ --prefix ] arg (=image)  prefix of files
		-o [ --out ] arg (=video)     destination video filename without extension
		-e [ --extension ] arg (=avi) destination video extension
		-f [ --fps ] arg (=15)        frame per seconds
		-c [ --fourcc ] arg (=LMP4)   fourcc code do use for encoding see: 
										http://www.fourcc.org/codecs.php for other 
										codecs
		-v [ --verbose ] arg (=0)     verbose level
		-m [ --video-mode ] arg (=1)  Video generation mode:
										1 -> two videos: one with rgb and the other 
										with alpha
										2 -> a video with double height: on top rgb on 
										bottom alpha
										3 -> a video with alpha channel trasformet as 
										green
                                
# About
This small utility create a video with different encoding 
startig from a sequence of image.

The images must have a common prefix followed by '_' 
character and a number.

This utility can generate different kind of video. You can choose with -m options.