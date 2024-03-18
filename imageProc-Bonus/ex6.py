def compare_pixel(pixel1, pixel2):
    distR = abs(pixel1[0]-pixel2[0])
    distG = abs(pixel1[1]-pixel2[1])
    distB = abs(pixel1[2]-pixel2[2])

    return distR + distB + distG


def compare(image1, image2):
    pass


def get_piece(image, upper_left, size):
    pass


def set_piece(image, upper_left, piece):
    pass


def average(image):
    pass


def preprocess_tiles(tiles):
    pass


def get_best_tiles(objective, tiles, averages , num_candidates):
    pass


def choose_tile(piece, tiles):
    pass


def make_mosaic(image, tiles, num_candidates):
    pass