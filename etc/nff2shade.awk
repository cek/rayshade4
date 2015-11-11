#
# This awk script will convert an NFF-format input file (as output by
# Eric Haines' SPD) to something rayshade can understand.
# The World object will be enclosed in a single grid of 22*22*22 voxels.
#
# Example usage:
#	mount | awk -f sped2shade.awk | rayshade > mountains.rle
#
# For best results, one should modify the output for all but the tetra
# and mountain databases to provide a tighter bounding box around the
# object of interest (tree, gears, etc.).  This is done by moving
# ground polygons and the like outside of the topmost grid object.
# This will decrease ray-tracing time dramatically.
#
# Note that we have to make sure that the viewing paramters are output
# outside of the object definition block.  We do this by printing
# the eye position, etc., at the very end of the output.
#
BEGIN{
	init = 0;
	lights = 0;
	print "maxdepth 4"
	print "sample 1 nojitter"
	print "cutoff 0."
	print "report verbose"
}
substr($1, 1, 1) == "#" { print "/* " $0 " */"; next;}
$1 == "v" { next;}
$1 == "from" { eyex = $2; eyey = $3; eyez = $4; next;}
$1 == "at" { atx = $2; aty = $3; atz = $4; next;}
$1 == "up" { upx = $2; upy = $3; upz = $4; next;}
$1 == "angle" { fov = $2; next;}
$1 == "hither" {next;}
$1 == "resolution" {screenx = $2; screeny = $3; next;}

$1 == "l" { lightd[lights] = $2 " "$3 " "$4; lights++; next; }
$1 == "b" {print "background " $2 " "$3 " "$4; next; }
$1 == "f" {
	if (init == 0) {
		print "grid 22 22 22";
		init = 1;
	}
	printf("applysurf ");
	if (lights != 0)
		aintens = sqrt(lights) / (4*lights);
	else
		aintens = .1;
	dr = $2*$5;
	dg = $3*$5;
	db = $4*$5;
# this is a good guess....
	ar = aintens*dr;
	ag = aintens*dg;
	ab = aintens*db;
	if (ar != 0 || ag != 0 || ab != 0)
		printf("\tambient %f %f %f\n", ar, ag, ab);
	if (dr != 0 || dg != 0 || db != 0)
		printf("\tdiffuse %f %f %f\n", dr, dg, db);
#
# This gets a little strange.  We're given a color, Ks, and T.
# We need a specular color, a specular reflectivity (for reflected
# rays), and a transparency (for transmitted rays).
# In rayshade, reflected rays have intensity proportional to
# specular_color*reflectivity, transmitted proportaional to
# specular_color*transparency, and specular hilights to
# specular_color.  Also, Ks + T >1 for some SPDs.
#
	if ($6) {
		sr = $2*$6;
		sg = $3*$6;
		sb = $4*$6;
		printf("\tspecular %f %f %f specpow %f\n", sr, sg, sb, $7);
	}
	if ($6 < 1. - $8)
		printf("\treflect 1.0\n");
	else
		printf("\treflect %f\n", 1. - $8);

	if ($8 || $9)
		printf("\ttransp %f index %f\n", $8, $9);
	next;
}

$1 == "c" {
	getline;
	x1 = $1;
	y1 = $2;
	z1 = $3;
	br = $4;
	getline;
	printf("cone %f %f %f %f %f %f %f %f\n", \
		br, x1, y1, z1, $4, $1, $2, $3);
	next;
}
$1 == "s" {
	print "sphere "$5 " "$2 " "$3 " "$4;
	next;
}
$1 == "pp" {
	if ($2 == 3)
		print "triangle ";
	else
		print "poly ";
	next;
}
$1 == "p" {
#
# Polygon -- the vertices will print out in the default statement.
# If there are three vertices, make it a triangle.
#
	if ($2 == 3)
		print "triangle ";
	else
		print "poly ";
	next;
}
{
# Matched nothing (or is a vertex data) -- print it.
	print;
	next;
}
END{
	print "end"
#
# Output light definitions.
#
	intens = sqrt(lights) / (lights);
	for (i = 0; i < lights; i++) {
		print "light " intens " point " lightd[i]
	}
	printf("eyep %g %g %g\n", eyex, eyey, eyez);
	printf("lookp %g %g %g\n", atx, aty, atz);
	printf("up %g %g %g\n", upx, upy, upz);
	printf("fov %g\n", fov);
	printf("screen %d %d\n", screenx, screeny);
}
