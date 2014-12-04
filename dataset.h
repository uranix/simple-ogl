#define GREEN_COLOR 0.0f, 1.0f, 0.0f
#define BLUE_COLOR 	0.0f, 0.0f, 1.0f
#define RED_COLOR   1.0f, 0.0f, 0.0f
#define GREY_COLOR  0.8f, 0.8f, 0.8f
#define BROWN_COLOR 0.5f, 0.5f, 0.0f
#define WHITE_COLOR 1.0f, 1.0f, 1.0f

const float vertexData[] =
{
	+1.0f, +1.0f, +1.0f,
	-1.0f, -1.0f, +1.0f,
	-1.0f, +1.0f, -1.0f,
	+1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	+1.0f, +1.0f, -1.0f,
	+1.0f, -1.0f, +1.0f,
	-1.0f, +1.0f, +1.0f,

	GREEN_COLOR,
	BLUE_COLOR,
	RED_COLOR,
	BROWN_COLOR,

	GREEN_COLOR,
	BLUE_COLOR,
	RED_COLOR,
	BROWN_COLOR,

    WHITE_COLOR,
    WHITE_COLOR,
    WHITE_COLOR,
    WHITE_COLOR,

    WHITE_COLOR,
    WHITE_COLOR,
    WHITE_COLOR,
    WHITE_COLOR,
};

const GLshort indexData[] =
{
	0, 1, 2,
	1, 0, 3,
	2, 3, 0,
	3, 2, 1,

	5, 4, 6,
	4, 5, 7,
	7, 6, 4,
	6, 7, 5,
};
