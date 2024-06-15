import mosaic
import sys


def compare_pixel(pixel1, pixel2):
    distR = abs(pixel1[0] - pixel2[0])
    distG = abs(pixel1[1] - pixel2[1])
    distB = abs(pixel1[2] - pixel2[2])

    return distR + distB + distG


def compare(image1, image2):
    total_dist = 0
    im1_list = image1
    im2_list = image2

    # find the sizes of the lists.
    row_size_1 = len(im1_list)
    col_size_1 = len(im1_list[0])

    row_size_2 = len(im2_list)
    col_size_2 = len(im2_list[0])

    # iterate over the smaller size and add the distances.
    for i in range(min(row_size_1, row_size_2)):
        for j in range(min(col_size_1, col_size_2)):
            total_dist += compare_pixel(im1_list[i][j], im2_list[i][j])

    return total_dist


def get_piece(image, upper_left, size):
    global slice_row, slice_col
    slice_row = 0
    slice_col = 0
    im_list = image

    # getting the size of an image
    pic_max_height = len(im_list)
    pic_max_width = len(im_list[0])

    # getting the row and col of left corner of the piece of the picture i need.
    row = upper_left[0]
    col = upper_left[1]

    # getting the piece size
    piece_height = size[0]
    piece_width = size[1]

    slice_row = row + piece_height

    # calculate if the size of a piece is between the bounds.
    if row + piece_height > pic_max_height:
        slice_row = pic_max_height

    sliced_piece = []

    # constructing the new image
    for i in range(row, slice_row):
        sliced_piece.append(im_list[i][col:col + piece_width])

    return sliced_piece


def set_piece(image, upper_left, piece):
    global slice_row, slice_col, pi_slice_row, pi_slice_col

    # image and piece lists
    im_list = image
    pi_list = piece

    # image sizes
    im_max_height = len(im_list)
    im_max_width = len(im_list[0])

    # piece sizes
    pi_max_height = len(pi_list)
    pi_max_width = len(pi_list[0])

    # left corner
    row = upper_left[0]
    col = upper_left[1]

    # slice from the image I will insert to
    slice_row = row + pi_max_height
    slice_col = col + pi_max_width

    # slice from the piece appropriate size
    pi_slice_row = pi_max_height
    pi_slice_col = pi_max_width

    # checking if I can fit the piece in the constraints
    if row + pi_max_height > im_max_height:
        slice_row = im_max_height
        # taking the new size of the piece
        pi_slice_row = slice_row - row

    if col + pi_max_width > im_max_width:
        slice_col = im_max_width
        # taking the new size of the piece
        pi_slice_col = slice_col - col

    pi_index = 0
    # take each list and copy slice of the piece to the image.
    for i in range(row, slice_row):
        im_list[i][col: slice_col] = pi_list[pi_index][0:pi_slice_col]
        pi_index += 1


def average(image):
    im_list = image

    avgR = 0
    avgG = 0
    avgB = 0

    # calculate the amount of pixels
    num_pixels = len(im_list) * len(im_list[0])

    # adding all the r,g,b values of an image together to find their avg.
    for i in im_list:
        for j in i:
            avgR += j[0]
            avgG += j[1]
            avgB += j[2]

    return avgR / num_pixels, avgG / num_pixels, avgB / num_pixels


def preprocess_tiles(tiles):
    avg_tiles = []
    # loop over the tiles and use the average function I created to find the tile average color.

    for i in range(0, len(tiles) - 1):
        avg_tiles.append(average(tiles[i]))

    return avg_tiles


def get_best_tiles(objective, tiles, averages, num_candidates):
    global curr_best_tile
    objective_avg = average(objective)

    best_tiles = []
    best_dist = []
    curr_smallest_dist = sys.maxsize

    # run on the amount of tiles we need
    for turn in range(num_candidates):
        # loop over the tiles
        for i in range(0, len(tiles) - 1):
            # find the distance between the tile average and the target file
            curr_distance = compare_pixel(objective_avg, averages[i])
            # if the distance is smaller than the current smallest that means we found a better candidates.
            if curr_distance < curr_smallest_dist and curr_distance not in best_dist:
                curr_smallest_dist = curr_distance
                curr_best_tile = tiles[i]

        # adding the tile to the tile list I will return
        best_tiles.append(curr_best_tile)
        best_dist.append(curr_smallest_dist)
        curr_smallest_dist = sys.maxsize

    return best_tiles


def choose_tile(piece, tiles):
    global current_smallest_dist, best_tile
    current_smallest_dist = sys.maxsize
    best_tile = tiles[0]

    # loop over the tiles
    for tile in tiles:
        current_dist = compare(tile, piece)  # get the dist between a piece and the current tile.
        if current_dist < current_smallest_dist:  # find the smallest dist between tiles.
            current_smallest_dist = current_dist
            best_tile = tile  # save the tile so I can return it.

    return best_tile


def make_mosaic(image, tiles, num_candidates):
    tile_height = len(tiles[0])
    tile_width = len(tiles[0][0])

    # get tile averages
    tile_avg = preprocess_tiles(tiles)

    tile_size = (tile_height, tile_width)

    # run in the image to get corners for tiles.
    for i in range(0, len(image), tile_height):
        for j in range(0, len(image[0]), tile_width):
            curr_piece = get_piece(image, (i, j), tile_size)  # get the curr piece I will replace.
            best_tiles = get_best_tiles(curr_piece, tiles, tile_avg, num_candidates)  # get the most similar tiles.
            best = choose_tile(curr_piece, best_tiles)  # choose the most similar.
            set_piece(image, (i, j), best)  # put the piece in the picture.

    return image


def main():
    # there 6 cmd line arguments.
    if len(sys.argv) < 6:
        print("Wrong number of parameters. The correct usage is:\nex6.py <image_source> <images_dir> <output_name> <tile_height> <num_candidates>")
    else:
        # first I will load tiles and the image
        img = mosaic.load_image(sys.argv[1])
        tiles = mosaic.build_tile_base(sys.argv[2], int(sys.argv[4]))
        image = make_mosaic(img, tiles, int(sys.argv[5]))  # then I will make the mosaic
        mosaic.save(image, sys.argv[3])


if __name__ == '__main__':
    main()
