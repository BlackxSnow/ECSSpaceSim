$input a_position
$output v_uv0

#include <bgfx_shader.sh>

void main()
{
    gl_Position = vec4(a_position, 1.0);
    v_uv0 = ((a_position + 1.0) / 2.0).xy;
}