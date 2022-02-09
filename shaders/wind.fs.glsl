#version 330

uniform sampler2D screen_texture;
uniform float time;
uniform float darken_screen_factor;

in vec2 texcoord;

layout(location = 0) out vec4 color;

vec2 distort(vec2 uv) 
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A1: HANDLE THE WIND DISTORTION HERE (you may want to try sin/cos)
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	float a = 0.05;
	float dx = cos(5*uv.y + 0.1 * time) * a;
	float dy = cos(5*uv.x + 0.1 * time) * a;

	if (uv.x > 0.5) {
		dx *= 1-uv.x;
	} 
	else if (uv.x < 0.5) {
		dx *= uv.x;
	}
	if (uv.y > 0.5) {
		dy *= 1-uv.y;
	} 
	else if (uv.y < 0.5) {
		dy *= uv.y;
	}
	uv.x += dx;
	uv.y += dy;
	return uv;
}

vec4 color_shift(vec4 in_color) 
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A1: HANDLE THE COLOR SHIFTING HERE (you may want to make it yellow-ish)
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	in_color += vec4(1.0, 1.0, 0.0, 0) / 8.0;
	return in_color;
}

vec4 fade_color(vec4 in_color) 
{
	if (darken_screen_factor > 0)
		in_color -= darken_screen_factor * vec4(0.8, 0.8, 0.8, 0);
	return in_color;
}

void main()
{
	vec2 coord = distort(texcoord);

    vec4 in_color = texture(screen_texture, coord);
    color = color_shift(in_color);
    color = fade_color(color);
}