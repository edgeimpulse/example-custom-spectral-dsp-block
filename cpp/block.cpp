// PASTE THIS FUNCTION INTO YOUR main.cpp FILE

int extract_psd_bands_features(signal_t *signal, matrix_t *output_matrix, void *config_ptr, const float frequency) {
    ei_dsp_config_psd_bands_t config = *((ei_dsp_config_psd_bands_t*)config_ptr);

    int ret;

    const float sampling_freq = frequency;

    // input matrix from the raw signal
    matrix_t input_matrix(signal->total_length / config.axes, config.axes);
    if (!input_matrix.buffer) {
        EIDSP_ERR(EIDSP_OUT_OF_MEM);
    }

    signal->get_data(0, signal->total_length, input_matrix.buffer);

    // scale the signal
    ret = numpy::scale(&input_matrix, config.scale_axes);
    if (ret != EIDSP_OK) {
        ei_printf("ERR: Failed to scale signal (%d)\n", ret);
        EIDSP_ERR(ret);
    }

    // transpose the matrix so we have one row per axis (nifty!)
    ret = numpy::transpose(&input_matrix);
    if (ret != EIDSP_OK) {
        ei_printf("ERR: Failed to transpose matrix (%d)\n", ret);
        EIDSP_ERR(ret);
    }

    // the spectral edges that we want to calculate
    matrix_t edges_matrix_in(((int)(sampling_freq / 2) + 1), 1);
    size_t edge_matrix_ix = 0;

    // 1 => sampling_freq / 2
    for (size_t ix = 1; ix < ((int)(sampling_freq / 2) + 1); ix++) {
        float begin = ((float)ix) - 0.5f;
        edges_matrix_in.buffer[edge_matrix_ix] = begin;

        edge_matrix_ix++;
    }

    edges_matrix_in.rows = edge_matrix_ix;

    // calculate how much room we need for the output matrix
    size_t output_matrix_cols = spectral::feature::calculate_spectral_buffer_size(
        true, 0, edges_matrix_in.rows
    );
    // ei_printf("output_matrix_size %hux%zu\n", input_matrix.rows, output_matrix_cols);
    if (output_matrix->cols * output_matrix->rows != static_cast<uint32_t>(output_matrix_cols * config.axes)) {
        ei_printf("output_matrix size is %dx%d\n",
            (int)output_matrix->rows, (int)output_matrix->cols);
        ei_printf("calculated size is %dx%d\n",
            (int)config.axes, (int)output_matrix_cols);

        EIDSP_ERR(EIDSP_MATRIX_SIZE_MISMATCH);
    }

    output_matrix->cols = output_matrix_cols;
    output_matrix->rows = config.axes;

    spectral::filter_t filter_type;
    if (strcmp(config.filter_type, "low") == 0) {
        filter_type = spectral::filter_lowpass;
    }
    else if (strcmp(config.filter_type, "high") == 0) {
        filter_type = spectral::filter_highpass;
    }
    else {
        filter_type = spectral::filter_none;
    }

    ret = spectral::feature::spectral_analysis(output_matrix, &input_matrix,
        sampling_freq, filter_type, config.filter_cutoff, config.filter_order,
        config.fft_length, 0, 0.0f, &edges_matrix_in);
    if (ret != EIDSP_OK) {
        ei_printf("ERR: Failed to calculate spectral features (%d)\n", ret);
        EIDSP_ERR(ret);
    }

    // flatten again
    output_matrix->cols = config.axes * output_matrix_cols;
    output_matrix->rows = 1;

    return EIDSP_OK;
}
