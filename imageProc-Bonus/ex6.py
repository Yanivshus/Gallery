import mosaic


def compare_pixel(pixel1, pixel2):
    distR = abs(pixel1[0] - pixel2[0])
    distG = abs(pixel1[1] - pixel2[1])
    distB = abs(pixel1[2] - pixel2[2])

    return distR + distB + distG


def compare(image1, image2):
    total_dist = 0
    im1_list = mosaic.lists_from_pil_image(image1)
    im2_list = mosaic.lists_from_pil_image(image2)

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

    slice_row = piece_height
    slice_col = piece_width

    # calculate if the size of a piece is between the bounds.
    if row + piece_height > pic_max_height:
        slice_row = pic_max_height
    if col + piece_width > pic_max_width:
        slice_col = pic_max_width

    sliced_piece = []

    # constructing the new image
    for i in range(row, slice_row - 1):
        sliced_piece.append(im_list[i][col:slice_col - 1])

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
    pass


def preprocess_tiles(tiles):
    pass


def get_best_tiles(objective, tiles, averages, num_candidates):
    pass


def choose_tile(piece, tiles):
    pass


def make_mosaic(image, tiles, num_candidates):
    pass
