uint pcg_hash(uint seed)
{
    uint state = seed * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

// Used to advance the PCG state.
uint rand_pcg(inout uint rng_state)
{
    uint state = rng_state;
    rng_state = rng_state * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

// Advances the prng state and returns the corresponding random float.
float rand(inout uint state)
{
    uint x = rand_pcg(state);
    state = x;
    return float(x) * uintBitsToFloat(0x2f800004u);
}

float rng(float min, float max, inout uint state)
{
    return rand(state) * (max - min) + min;
}

vec2 rng(vec2 min, vec2 max, inout uint state)
{
    return vec2(rng(min.x, max.x, state), rng(min.y, max.y, state));
}


vec3 rng(vec3 min, vec3 max, inout uint state)
{
    return vec3(rng(min.x, max.x, state), rng(min.y, max.y, state), rng(min.z, max.z, state));
}


vec4 rng(vec4 min, vec4 max, inout uint state)
{
    return vec4(rng(min.x, max.x, state), rng(min.y, max.y, state), rng(min.z, max.z, state), rng(min.w, max.w, state));
}
