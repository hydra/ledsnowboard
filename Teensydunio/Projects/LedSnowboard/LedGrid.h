

extern const uint16_t ledGridToIndexMapping[GRID_WIDTH * GRID_HEIGHT];

class LedGrid {
public:
	LedGrid(uint8_t width, uint8_t height, const uint16_t *mapping);

	uint16_t retrieveLedNumberByPosition(uint8_t x, uint8_t y);

private:
	uint8_t width;
	uint8_t height;
	const uint16_t *mapping;
};
