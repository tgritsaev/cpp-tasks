#pragma once

#include "fft.h"
#include "utils/image.h"
#include "huffman.h"
#include <istream>
#include <stdexcept>
#include <vector>
#include <math.h>
#include <memory>
#include <glog/logging.h>

class MarkReader {
public:
    MarkReader(std::istream& input);

    Image& GetImage() {
        return final_image_;
    }

private:
    inline int ReadTwoBytesAsOne(std::istream& input) {
        return input.get() * 256 + input.get();
        // if (input.peek() != EOF) {
        //     int a = input.get();
        //     if (input.peek() != EOF) {
        //         return a * 256 + input.get();
        //     } else {
        //         throw std::invalid_argument("not enough");
        //     }
        // } else {
        //     throw std::invalid_argument("not enough");
        // }
    }

    inline int ReadOneByteOrThrow(std::istream& input) {
        if (input.peek() == EOF) {
            throw std::invalid_argument("bad");
        }
        return input.get();
    }

    void DeadReadOnlyForDebug(std::istream& input) {
        while (input.peek() != EOF) {
            std::string log_string;
            log_string += std::to_string(input.get()) + " ";
            if (log_string == "255 " && input.peek() != EOF) {
                log_string += std::to_string(input.get());
                DLOG(INFO) << log_string;
            }
        }
    }

    template <typename T>
    void DLOGVectorOnlyForDebug(const std::string& info, const std::vector<T>& vector_info) {
        DLOG(INFO) << info;
        std::string log_string;
        for (int i = 0; i < 64; ++i) {
            log_string += std::to_string(static_cast<int>(vector_info[i])) + "\t";
            if (i % 8 == 7) {
                DLOG(INFO) << log_string;
                log_string = "";
            }
        }
        DLOG(INFO);
    }

    void ReadStartMarker(std::istream& input);

    void ReadEmpty(std::istream& input);

    void ReadCommentMarker(std::istream& input);

    void ReadDQTMarker(std::istream& input);

    void ReadSOF0Marker(std::istream& input);

    void ReadDHTMarker(std::istream& input);

    void ReadSOSMarker(std::istream& input);

    void CreateZigZagOrder();

    static inline std::vector<size_t> zig_zag_order;
    std::vector<std::vector<int>> dqt_tables_;

    struct SOF0Channel {
        size_t id;
        size_t H, V;
        size_t subsampling_H, subsampling_V;
        size_t dqt_id;
    };
    size_t precision_;
    size_t height_{0};
    size_t width_{0};
    size_t channels_cnt_{0};
    size_t h_max_;
    size_t v_max_;
    bool is_visited_sof_{false};
    std::vector<SOF0Channel> sof_channels_;

    enum CoefTableType { DC = 0, AC = 1 };

    std::unique_ptr<HuffmanTree> dht_huffman_trees_[3][2];

    size_t sos_channels_ids_[3][2];

    struct MCUMovingStatus;
    struct MCUBlock {
        std::vector<std::vector<double>> y;
        std::vector<std::vector<double>> cb;
        std::vector<std::vector<double>> cr;
    };
    std::vector<MCUBlock> mcu_blocks_;

    Image final_image_;
    bool was_sos_marker_{false};
    bool end_of_file_{false};
};

Image Decode(std::istream& input);

constexpr int kDebugOther = 0;
constexpr int kDebugDQT = 0;
constexpr int kDebugSOF0 = 0;
constexpr int kDebugDHT = 0;
constexpr int kDebugSOS = 0;
constexpr int kDebugZigZagTable = 0;
