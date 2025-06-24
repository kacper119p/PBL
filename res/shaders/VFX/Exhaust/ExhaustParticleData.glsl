struct particle {
    vec3 position;
    float scale;
    vec3 velocity;
    float life;
    float rotation;
    float[3] padding;
};

const int SPRITE_COLUMNS = 8;
const int SPRITE_ROWS = 8;
const int TOTAL_FRAMES = SPRITE_COLUMNS * SPRITE_ROWS;
const float MAX_LIFETIME = 2.6666666666666666666666666666666666;
const float FRAME_DURATION = MAX_LIFETIME / float(TOTAL_FRAMES);
