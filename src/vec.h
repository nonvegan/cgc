#ifndef VEC_H_
#define VEC_H_

typedef struct {
    int x, y;
} Vec2i;

Vec2i vec2i(int x, int y);
Vec2i vec2ii(int x);
Vec2i vec2i_add(Vec2i a, Vec2i b);
Vec2i vec2i_sub(Vec2i a, Vec2i b);
Vec2i vec2i_mul(Vec2i a, Vec2i b);
Vec2i vec2i_div(Vec2i a, Vec2i b);


typedef struct {
    float x, y;
} Vec2f;

Vec2f vec2f(float x, float y);
Vec2f vec2ff(float x);
Vec2f vec2f_add(Vec2f a, Vec2f b);
Vec2f vec2f_sub(Vec2f a, Vec2f b);
Vec2f vec2f_mul(Vec2f a, Vec2f b);
Vec2f vec2f_div(Vec2f a, Vec2f b);

#endif // VEC_H_
