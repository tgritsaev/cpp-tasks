#include "decoder.h"

MarkReader::MarkReader(std::istream& input) {
    CreateZigZagOrder();

    while (input.peek() != EOF) {
        int first = input.get();
        int second = input.get();

        if (first == 255 && second == 216) {
            // 255 = FF and D8 = 216
            ReadStartMarker(input);
        } else if (first == 255 && second == 254) {
            // 255 = FF and should be (254 = FE) by habr [ but also 224, 225 ]
            ReadCommentMarker(input);
        } else if (first == 255 && second == 219) {
            // 255 = FF and DB = 219
            ReadDQTMarker(input);
        } else if (first == 255 && second == 192) {
            // 255 = FF and 192 = CO
            ReadSOF0Marker(input);
        } else if (first == 255 && second == 196) {
            // 255 = FF and 196 = C4
            ReadDHTMarker(input);
        } else if (first == 255 && second == 218) {
            // 255 = FF and 218 = DA
            ReadSOSMarker(input);
            break;
        } else if (first == 255 && (second == 224 || second == 225 || second == 226 ||
                                    second == 236 || second == 238)) {
            ReadEmpty(input);
        } else {
            throw std::invalid_argument("no such marker with first=" + std::to_string(first) +
                                        " second=" + std::to_string(second));
        }
    }
    if (final_image_.Height() == 0 || final_image_.Width() == 0 || end_of_file_ == false) {
        throw std::invalid_argument("worked incorrect");
    }
}

void MarkReader::ReadStartMarker(std::istream& input) {
    (void)input;
    DLOG_IF(INFO, kDebugOther == 1) << "ReadStartMarker";
}

void MarkReader::ReadEmpty(std::istream& input) {
    DLOG_IF(INFO, kDebugOther == 1) << "ReadEmpty";
    int len = ReadTwoBytesAsOne(input) - 2;
    // comment code below in production
    for (int i = 0; i < len; ++i) {
        input.get();
    }
}

void MarkReader::ReadCommentMarker(std::istream& input) {
    DLOG_IF(INFO, kDebugOther == 1) << "ReadCommentMarker";
    int len1 = input.get();
    int len2 = input.get();
    if (len1 != 0) {
        throw std::invalid_argument("len1 != 0 => don't know what to do");
    }
    int len = len2 - 2;
    // comment code below in production
    std::string comment;
    for (int i = 0; i < len; ++i) {
        char c = input.get();
        comment += c;
    }
    if (comment == "JFIFHH") {
        comment = "";
    }
    DLOG_IF(INFO, kDebugOther == 1) << "comment of image: " << comment;
    final_image_.SetComment(comment);
}

void MarkReader::ReadDQTMarker(std::istream& input) {
    DLOG_IF(INFO, kDebugOther == 1) << "ReadDQTMarker";
    int len = ReadTwoBytesAsOne(input) - 2;
    if (len % 65 != 0) {
        throw std::invalid_argument("len % 65 != 0, no realization " + std::to_string(len));
    }
    for (int i = 0; i < len / 65; ++i) {
        int identifier = input.get();
        if (identifier / 16 >= 1) {
            throw std::invalid_argument("identifier / 16 >= 1, no realization");
        }

        dqt_tables_.push_back({});
        dqt_tables_.back().resize(64);
        for (int i = 0; i < 64; ++i) {
            int c = input.get();
            dqt_tables_.back()[zig_zag_order[i]] = c;
        }
        if constexpr (kDebugDQT >= 1) {
            DLOGVectorOnlyForDebug("DQT #" + std::to_string(dqt_tables_.size()),
                                   dqt_tables_.back());
        }
    }
}

void MarkReader::ReadSOF0Marker(std::istream& input) {
    DLOG_IF(INFO, kDebugOther == 1) << "ReadSOF0Marker";
    if (is_visited_sof_ == true) {
        throw std::invalid_argument("visited twice");
    }
    is_visited_sof_ = true;
    int len = ReadTwoBytesAsOne(input) - 2;
    // if (len != 15) {
    //     throw std::invalid_argument("len != 15");
    // }
    precision_ = input.get();
    if (precision_ != 8) {
        throw std::invalid_argument("precision should be 8");
    }
    height_ = ReadTwoBytesAsOne(input);
    width_ = ReadTwoBytesAsOne(input);
    channels_cnt_ = input.get();
    sof_channels_.resize(channels_cnt_);
    h_max_ = 0;
    v_max_ = 0;
    for (size_t i = 0; i < channels_cnt_; ++i) {
        sof_channels_[i].id = input.get();
        int thinning = input.get();
        sof_channels_[i].H = thinning / 16;
        sof_channels_[i].V = thinning % 16;
        sof_channels_[i].dqt_id = input.get();

        h_max_ = std::max<int>(h_max_, sof_channels_[i].H);
        v_max_ = std::max<int>(v_max_, sof_channels_[i].V);
    }
    for (size_t i = 0; i < channels_cnt_; ++i) {
        if (sof_channels_[i].H == 0 || sof_channels_[i].V == 0 ||
            h_max_ % sof_channels_[i].H != 0 || v_max_ % sof_channels_[i].V) {
            throw std::invalid_argument(
                "h_max '%' sof_channels_[i].H != 0 || v_max '%' sof_channels_[i].V");
        }
        sof_channels_[i].subsampling_H = h_max_ / sof_channels_[i].H;
        sof_channels_[i].subsampling_V = v_max_ / sof_channels_[i].V;
    }
}

void MarkReader::ReadDHTMarker(std::istream& input) {
    DLOG_IF(INFO, kDebugOther == 1) << "ReadDHTMarker";

    int len = ReadTwoBytesAsOne(input) - 2;
    while (len > 0) {
        int ac_dc_id = input.get();
        CoefTableType now_type;
        if (ac_dc_id >= 16) {
            now_type = AC;
        } else {
            now_type = DC;
        }
        int id = ac_dc_id % 16;
        if (id >= 3 || id < 0) {
            throw std::invalid_argument("ReadDHTMarker id >= 3");
        }
        std::unique_ptr<HuffmanTree>& now_huffman_tree = dht_huffman_trees_[id][now_type];

        std::vector<uint8_t> code_lengths(16);
        size_t cnt = 0;
        for (size_t i = 0; i < 16; ++i) {
            code_lengths[i] = ReadOneByteOrThrow(input);
            cnt += code_lengths[i];
        }
        std::vector<uint8_t> values(cnt);
        for (size_t i = 0; i < cnt; ++i) {
            values[i] = ReadOneByteOrThrow(input);
        }
        len -= (cnt + 16 + 1);
        now_huffman_tree = std::make_unique<HuffmanTree>();
        now_huffman_tree->Build(code_lengths, values);
    }
    if (len < 0) {
        throw std::invalid_argument("invalid ReadDHTMarker");
    }
}

void MarkReader::ReadSOSMarker(std::istream& input) {
    DLOG_IF(INFO, kDebugOther == 1) << "ReadSOSMarker";

    if (is_visited_sof_ == false) {
        throw std::invalid_argument("sof not visited");
    }
    int len = ReadTwoBytesAsOne(input) - 2;
    int channels_cnt = input.get();
    for (int i = 0; i < channels_cnt; ++i) {
        int id = input.get() - 1;
        if (id >= 3 || id < 0) {
            throw std::invalid_argument("too big id");
        }
        int dc_ac_ids = input.get();
        sos_channels_ids_[id][DC] = dc_ac_ids / 16;
        sos_channels_ids_[id][AC] = dc_ac_ids % 16;
    }

    // progressive variables
    if (!(input.get() == 0 && input.get() == 63 && input.get() == 0)) {
        throw std::invalid_argument("ReadSOSMarker, incorrect 00 3F 00");
    }

    // navigating in tables

    size_t mcu_width = 8 * h_max_;
    size_t mcu_height = 8 * v_max_;
    // mcu_width = 8;
    // mcu_height = 8;

    int new_height = height_ + (height_ % 8 != 0 ? 8 - height_ % 8 : 0);
    int new_width = width_ + (width_ % mcu_width != 0 ? mcu_width - width_ % mcu_width : 0);

    mcu_width = 8 * h_max_;
    mcu_height = 8 * v_max_;
    if (new_height < 0 || new_width < 0) {
        throw std::invalid_argument("(new_height < 0 || new_width < 0) ");
    }
    size_t mcu_cnt = static_cast<long long>(new_height) * new_width / (mcu_width * mcu_height);
    if (mcu_cnt > 1000 * 1000) {
        throw std::invalid_argument("too big");
    }

    size_t cnt_y_tables_in_mcu = sof_channels_[0].H * sof_channels_[0].V;
    size_t cnt_cb_tables_in_mcu =
        (1 >= channels_cnt_ ? 0 : sof_channels_[1].H * sof_channels_[1].V);
    size_t cnt_cr_tables_in_mcu =
        (2 >= channels_cnt_ ? 0 : sof_channels_[2].H * sof_channels_[2].V);

    DLOG_IF(INFO, kDebugSOS > 2) << "channels_cnt_=" << channels_cnt_;
    DLOG_IF(INFO, kDebugSOS > 0) << "new_height=" << new_height << "\tnew_width=" << new_width
                                 << "\tmcu_cnt=" << mcu_cnt;
    DLOG_IF(INFO, kDebugSOS > 0) << "cnt_y_tables_in_mcu=" << cnt_y_tables_in_mcu
                                 << "\tcnt_cb_tables_in_mcu=" << cnt_cb_tables_in_mcu
                                 << "\tcnt_cr_tables_in_mcu=" << cnt_cr_tables_in_mcu;

    size_t now_mcu_num = 0;
    size_t now_table_inside_mcu_num = 0;
    int now_channel_num = 0;
    CoefTableType now_tree_type = DC;
    int now_huffman_tree_id;

    size_t written_in_table_values_cnt = 0;

    int now_number = input.get();
    int shift_bit = 7;
    int now_bit = 0;

    auto update_now_bit_shift_bit_and_probably_now_number = [this, &input, &shift_bit, &now_bit,
                                                             &now_number]() {
        if (end_of_file_) {
            return;
        }
        now_bit = ((now_number & (1 << shift_bit)) ? 1 : 0);
        --shift_bit;
        if (shift_bit < 0) {
            shift_bit = 7;
            now_number = input.get();
            if (now_number == 255) {
                int c = input.get();
                if (c == 217) {
                    end_of_file_ = true;
                } else if (c != 0) {
                    throw std::invalid_argument("oh no");
                }
            }
        }
    };

    auto write_value_in_corresponding_table =
        [&written_in_table_values_cnt](std::vector<double>* now_table_inside_mcu, int value) {
            size_t idx = zig_zag_order[written_in_table_values_cnt];
            (*now_table_inside_mcu)[idx] = value;
            ++written_in_table_values_cnt;
        };

    auto calculate_coef_in_third_item = [&update_now_bit_shift_bit_and_probably_now_number,
                                         &now_bit](int& coef, int coef_len) {
        update_now_bit_shift_bit_and_probably_now_number();
        int first_bit = now_bit;
        coef = now_bit;
        if (coef_len > 30) {
            throw std::invalid_argument("coef_len too big");
        }
        for (int i = 1; i < coef_len; ++i) {
            update_now_bit_shift_bit_and_probably_now_number();
            coef = (coef << 1) + now_bit;
        }
        if (first_bit == 0) {
            coef = coef - (1 << coef_len) + 1;
        }
    };

    mcu_blocks_.resize(mcu_cnt);
    for (size_t i = 0; i < mcu_cnt; ++i) {
        mcu_blocks_[i].y.resize(cnt_y_tables_in_mcu, std::vector<double>(64));
        mcu_blocks_[i].cb.resize(cnt_cb_tables_in_mcu, std::vector<double>(64));
        mcu_blocks_[i].cr.resize(cnt_cr_tables_in_mcu, std::vector<double>(64));
    }
    auto get_now_table_inside_mcu = [this, &mcu_cnt, &cnt_y_tables_in_mcu, &cnt_cb_tables_in_mcu,
                                     &cnt_cr_tables_in_mcu](size_t channel_num, size_t mcu_num,
                                                            size_t table_inside_mcu_num) {
        if (mcu_num == mcu_blocks_.size()) {
            ++mcu_cnt;
            mcu_blocks_.push_back({});
            mcu_blocks_.back().y.resize(cnt_y_tables_in_mcu, std::vector<double>(64));
            mcu_blocks_.back().cb.resize(cnt_cb_tables_in_mcu, std::vector<double>(64));
            mcu_blocks_.back().cr.resize(cnt_cr_tables_in_mcu, std::vector<double>(64));
        }
        size_t i;
        if (channel_num == 0) {
            i = table_inside_mcu_num;
            return &mcu_blocks_[mcu_num].y[i];
        } else if (channel_num == 1) {
            i = table_inside_mcu_num - cnt_y_tables_in_mcu;
            return &mcu_blocks_[mcu_num].cb[i];
        } else {
            i = table_inside_mcu_num - cnt_y_tables_in_mcu - cnt_cb_tables_in_mcu;
            return &mcu_blocks_[mcu_num].cr[i];
        }
    };

    // reading bits
    int previous_coef[3];
    previous_coef[0] = 0;
    previous_coef[1] = 0;
    previous_coef[2] = 0;
    size_t start_mcu_cnt = mcu_cnt;
    while (end_of_file_ == false) {

        if (start_mcu_cnt * 2 < mcu_cnt) {
            throw std::invalid_argument("too long while");
        }
        update_now_bit_shift_bit_and_probably_now_number();
        if (end_of_file_) {
            break;
        }
        std::vector<double>* now_table_inside_mcu =
            get_now_table_inside_mcu(now_channel_num, now_mcu_num, now_table_inside_mcu_num);

        now_huffman_tree_id = sos_channels_ids_[now_channel_num][now_tree_type];
        int huffman_tree_value = 0;
        if (now_huffman_tree_id < 0 || now_huffman_tree_id >= 3 ||
            dht_huffman_trees_[now_huffman_tree_id][now_tree_type] == nullptr) {
            throw std::invalid_argument(
                "incorrect addressing to dht_huffman_trees_[now_huffman_tree_id][now_tree_type]");
        }
        if (dht_huffman_trees_[now_huffman_tree_id][now_tree_type]->Move(
                now_bit, huffman_tree_value) == true) {
            DLOG_IF(INFO, kDebugSOS > 2) << "huffman_tree_value=" << huffman_tree_value;

            if (now_tree_type == DC) {
                int dc_coef = 0;
                if (huffman_tree_value > 0) {
                    calculate_coef_in_third_item(dc_coef, huffman_tree_value);
                }
                dc_coef += previous_coef[now_channel_num];
                previous_coef[now_channel_num] = dc_coef;
                write_value_in_corresponding_table(now_table_inside_mcu, dc_coef);
                DLOG_IF(INFO, kDebugSOS > 2) << "dc_coef=" << dc_coef;
                now_tree_type = AC;
            } else {
                if (huffman_tree_value == 0) {
                    while (written_in_table_values_cnt < 64) {
                        write_value_in_corresponding_table(now_table_inside_mcu, 0);
                    }
                } else {
                    int ac_coef = 0;

                    int zeros_cnt = huffman_tree_value / 16;
                    while (zeros_cnt > 0 && written_in_table_values_cnt < 64) {
                        write_value_in_corresponding_table(now_table_inside_mcu, 0);
                        --zeros_cnt;
                    }
                    int coef_len = huffman_tree_value % 16;
                    if (written_in_table_values_cnt < 64) {
                        if (coef_len > 0) {
                            calculate_coef_in_third_item(ac_coef, coef_len);
                        }
                        write_value_in_corresponding_table(now_table_inside_mcu, ac_coef);
                        DLOG_IF(INFO, kDebugSOS > 2) << "ac_coef=" << ac_coef;
                    }
                }
            }

            if (written_in_table_values_cnt == 64) {
                if constexpr (kDebugSOS > 1) {
                    DLOGVectorOnlyForDebug("before quantization, now_table_num=" +
                                               std::to_string(now_table_inside_mcu_num),
                                           *now_table_inside_mcu);
                }
                ++now_table_inside_mcu_num;
                now_tree_type = DC;
                written_in_table_values_cnt = 0;
                if (now_table_inside_mcu_num ==
                    cnt_y_tables_in_mcu + cnt_cb_tables_in_mcu + cnt_cr_tables_in_mcu) {
                    ++now_mcu_num;
                    now_table_inside_mcu_num = 0;
                    now_channel_num = 0;
                } else if (now_table_inside_mcu_num == cnt_y_tables_in_mcu) {
                    ++now_channel_num;
                } else if (now_table_inside_mcu_num == cnt_y_tables_in_mcu + cnt_cb_tables_in_mcu) {
                    ++now_channel_num;
                }
            }
        }

        now_table_inside_mcu = nullptr;
    }
    if (now_mcu_num + 1 < mcu_cnt) {
        throw std::invalid_argument("not enough mcu");
    }
    // here we have tables

    for (size_t i = 0; i < std::min(mcu_cnt, mcu_blocks_.size()); ++i) {
        auto inplace_multiply_table_on_dqt = [](std::vector<double>& table,
                                                const std::vector<int>& dqt) {
            for (int i = 0; i < 64; ++i) {
                table[i] *= dqt[i];
            }
        };
        int id = sof_channels_[0].dqt_id;
        if (channels_cnt_ > 0) {
            if (id < 0 || static_cast<size_t>(id) >= dqt_tables_.size()) {
                throw std::invalid_argument(
                    "id < 0 || static_cast<size_t>(id) >= dqt_tables_.size()");
            }
            for (size_t j = 0; j < std::min(mcu_blocks_[i].y.size(), cnt_y_tables_in_mcu); ++j) {
                inplace_multiply_table_on_dqt(mcu_blocks_[i].y[j], dqt_tables_[id]);
            }
        }
        if (channels_cnt_ > 1) {
            id = sof_channels_[1].dqt_id;
            if (id < 0 || static_cast<size_t>(id) >= dqt_tables_.size()) {
                throw std::invalid_argument(
                    "id < 0 || static_cast<size_t>(id) >= dqt_tables_.size()");
            }
            for (size_t j = 0; j < std::min(cnt_cb_tables_in_mcu, mcu_blocks_[i].cb.size()); ++j) {
                inplace_multiply_table_on_dqt(mcu_blocks_[i].cb[j], dqt_tables_[id]);
            }
        }
        if (channels_cnt_ > 2) {
            id = sof_channels_[2].dqt_id;
            if (id < 0 || static_cast<size_t>(id) >= dqt_tables_.size()) {
                throw std::invalid_argument(
                    "id < 0 || static_cast<size_t>(id) >= dqt_tables_.size()");
            }
            for (size_t j = 0; j < cnt_cr_tables_in_mcu; ++j) {
                inplace_multiply_table_on_dqt(mcu_blocks_[i].cr[j], dqt_tables_[id]);
            }
        }
    }
    // here we have quantizied tables

    std::vector<double> dct_values(64);
    DctCalculator dct_calculator(8, &dct_values, &dct_values);
    for (size_t i = 0; i < mcu_cnt; ++i) {
        auto apply_dct = [&dct_calculator, &dct_values](std::vector<double>& table) {
            for (int i = 0; i < 64; ++i) {
                dct_values[i] = table[i];
            }
            dct_calculator.Inverse();

            for (int j = 0; j < 64; ++j) {
                table[j] = std::min(std::max(0, static_cast<int>(round(dct_values[j])) + 128), 255);
            }
        };
        for (size_t j = 0; j < cnt_y_tables_in_mcu; ++j) {
            apply_dct(mcu_blocks_[i].y[j]);
        }
        for (size_t j = 0; j < cnt_cb_tables_in_mcu; ++j) {
            apply_dct(mcu_blocks_[i].cb[j]);
        }
        for (size_t j = 0; j < cnt_cr_tables_in_mcu; ++j) {
            apply_dct(mcu_blocks_[i].cr[j]);
        }
    }

    final_image_.SetSize(width_, height_);
    size_t start_y = 0;
    size_t start_x = 0;
    DLOG_IF(INFO, kDebugOther > 0) << "pre: YCbCR -> RGB";
    for (size_t mcu_id = 0; mcu_id < std::min(mcu_cnt, mcu_blocks_.size()); ++mcu_id) {
        if (mcu_id > 0) {
            start_x += mcu_width;
        }
        if (start_x >= width_) {
            start_x = 0;
            start_y += mcu_height;
        }

        for (size_t y = 0; y < mcu_height && start_y + y < height_; ++y) {
            for (size_t x = 0; x < mcu_width && start_x + x < width_; ++x) {
                auto get_value = [this, &y, &x, &mcu_width](
                                     const std::vector<std::vector<double>>& channel_tables,
                                     size_t channel_num) {
                    int y_after_sub = y / sof_channels_[channel_num].subsampling_V;
                    int x_after_sub = x / sof_channels_[channel_num].subsampling_H;

                    size_t y = (y_after_sub / 8) * (mcu_width / 8) + (x_after_sub / 8);
                    size_t x = (y_after_sub % 8) * 8 + (x_after_sub % 8);

                    if (y >= channel_tables.size() || x >= channel_tables[y].size()) {
                        throw std::invalid_argument(":( :(");
                    }

                    return channel_tables[y][x];
                };
                RGB pixel;

                double y_value = get_value(mcu_blocks_[mcu_id].y, 0);
                if (channels_cnt_ == 3) {
                    double cb_value = get_value(mcu_blocks_[mcu_id].cb, 1);
                    double cr_value = get_value(mcu_blocks_[mcu_id].cr, 2);

                    pixel.r = round(y_value + 1.402 * (cr_value - 128));
                    pixel.g =
                        round(y_value - 0.344136 * (cb_value - 128) - 0.714136 * (cr_value - 128));
                    pixel.b = round(y_value + 1.772 * (cb_value - 128));

                    pixel.r = std::min(std::max(0, pixel.r), 255);
                    pixel.g = std::min(std::max(0, pixel.g), 255);
                    pixel.b = std::min(std::max(0, pixel.b), 255);
                } else {
                    pixel.r = std::min(std::max(0, static_cast<int>(round(y_value))), 255);
                    pixel.g = pixel.r;
                    pixel.b = pixel.r;
                }
                final_image_.SetPixel(start_y + y, start_x + x, pixel);
            }
        }
    }
    if (kDebugSOS > 0) {
        DLOG(INFO) << "RED";
        for (size_t y = 0; y < final_image_.Height(); ++y) {
            std::string log_string;
            for (size_t x = 0; x < final_image_.Width(); ++x) {
                log_string += std::to_string(final_image_.GetPixel(y, x).r) + " ";
            }
            DLOG(INFO) << log_string;
            log_string = "";
        }
        DLOG(INFO) << "GREEN";
        for (size_t y = 0; y < final_image_.Height(); ++y) {
            std::string log_string;
            for (size_t x = 0; x < final_image_.Width(); ++x) {
                log_string += std::to_string(final_image_.GetPixel(y, x).g) + " ";
            }
            DLOG(INFO) << log_string;
            log_string = "";
        }
        DLOG(INFO) << "BLUE";
        for (size_t y = 0; y < final_image_.Height(); ++y) {
            std::string log_string;
            for (size_t x = 0; x < final_image_.Width(); ++x) {
                log_string += std::to_string(final_image_.GetPixel(y, x).b) + " ";
            }
            DLOG(INFO) << log_string;
            log_string = "";
        }
    }
}

void MarkReader::CreateZigZagOrder() {
    DLOG_IF(INFO, kDebugZigZagTable == 1) << "CreateZigZagOrder";
    if (zig_zag_order.empty() == false) {
        return;
    }

    std::vector<std::pair<int, int>> first_type_move = {{0, 1}, {1, 0}};
    std::vector<std::pair<int, int>> second_type_move = {{1, -1}};
    std::vector<std::pair<int, int>> third_type_move = {{1, 0}, {0, 1}};
    std::vector<std::pair<int, int>> forth_type_move = {{-1, 1}};

    std::vector<std::vector<std::pair<int, int>>> moves = {first_type_move, second_type_move,
                                                           third_type_move, forth_type_move};
    int x = 0;
    int y = 0;
    int type_move = 0;
    auto get_pos = [&x, &y]() { return x * 8 + y; };
    auto get_move = [&moves, &type_move, &get_pos]() {
        if (type_move == 1 || type_move == 3) {
            return moves[type_move].front();
        } else {
            auto pos = get_pos();
            DLOG_IF(INFO, kDebugZigZagTable == 1) << pos << " " << type_move;
            if ((56 <= pos && pos <= 64) || (pos % 8 == 7 && pos >= 15)) {
                return moves[type_move].back();
            } else {
                return moves[type_move].front();
            }
        }
    };
    auto next_position = [&x, &y](std::pair<int, int> move) {
        return std::make_pair<int, int>(x + move.first, y + move.second);
    };
    auto is_valid_position = [](int x) { return (0 <= x && x <= 7); };
    zig_zag_order.resize(64);

    for (size_t i = 0; i < 64; ++i) {
        DLOG_IF(INFO, kDebugZigZagTable == 1) << "current position: " << x << " " << y;
        zig_zag_order[i] = get_pos();

        auto move = get_move();
        auto [next_x, next_y] = next_position(move);
        if (is_valid_position(next_x) && is_valid_position(next_y)) {
            x = next_x;
            y = next_y;
        } else {
            type_move = (type_move + 1) % 4;
            auto next_pos = next_position(get_move());
            x = next_pos.first;
            y = next_pos.second;
        }
        type_move += (type_move == 0 || type_move == 2);
    }
    if constexpr (kDebugZigZagTable > 0) {
        for (size_t i = 0; i < 64; ++i) {
            DLOG(INFO) << i << ": " << zig_zag_order[i];
        }
    }
    // i thought ZigZag creation would be easier
}

Image Decode(std::istream& input) {
    DLOG_IF(INFO, kDebugOther == 1) << "Decode started";

    MarkReader mark_reader(input);

    DLOG_IF(INFO, kDebugOther == 1) << "Decode ended";

    return mark_reader.GetImage();
}