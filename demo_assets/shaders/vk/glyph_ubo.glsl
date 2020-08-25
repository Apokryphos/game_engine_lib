const uint MAX_GLYPHS = 200;

struct Glyph
{
    mat4 model;
    vec4 bg_color;
    vec4 fg_color;
    uint texture_index;
};

layout(set = 2, binding = 0, std140) uniform GlyphUBO
{
    layout(align = 16) Glyph glyphs[MAX_GLYPHS];
} glyph_ubo;
