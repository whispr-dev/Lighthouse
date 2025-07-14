#include <iostream>
#include <vector>
#include <optional>
#include <cstdint>
#include <bitset>

struct Row {
    int a;
    int b;
    std::optional<int> c; // may be missing

    friend std::ostream& operator<<(std::ostream& os, const Row& r) {
        os << "{a: " << r.a << ", b: " << r.b << ", c: ";
        if (r.c) os << *r.c; else os << "null";
        os << "}";
        return os;
    }

    bool operator==(const Row& other) const {
        return a == other.a && b == other.b && c == other.c;
    }
};

struct DeltaBlock {
    size_t row_number;
    uint8_t missing_bitmap; // 1 = a missing, 2 = b missing, 4 = c missing
    std::optional<int> delta_a, delta_b, delta_c;

    void print() const {
        std::cout << "Row " << row_number
                  << " missing: " << std::bitset<3>(missing_bitmap)
                  << " deltas: (";
        if (delta_a) std::cout << *delta_a; else std::cout << "-";
        std::cout << ", ";
        if (delta_b) std::cout << *delta_b; else std::cout << "-";
        std::cout << ", ";
        if (delta_c) std::cout << *delta_c; else std::cout << "-";
        std::cout << ")\n";
    }
};

// Delta encode: stores "new value if changed", and sets bits for missing fields
std::vector<DeltaBlock> delta_encode(const std::vector<Row>& rows) {
    std::vector<DeltaBlock> out;
    Row prev = rows.front();
    for (size_t i = 0; i < rows.size(); ++i) {
        const Row& cur = rows[i];
        DeltaBlock d;
        d.row_number = i;
        d.missing_bitmap = 0;
        // a
        if (cur.a != prev.a) d.delta_a = cur.a;
        // b
        if (cur.b != prev.b) d.delta_b = cur.b;
        // c (optional)
        if (!cur.c.has_value()) d.missing_bitmap |= 0x4;
        else if (cur.c != prev.c) d.delta_c = cur.c;

        out.push_back(d);
        prev = cur;
    }
    return out;
}

// Reconstruct from base row and deltas
std::vector<Row> reconstruct_rows(const Row& base, const std::vector<DeltaBlock>& deltas) {
    std::vector<Row> output;
    Row prev = base;
    for (const auto& block : deltas) {
        Row next = prev;
        // a
        if (block.delta_a) next.a = *block.delta_a;
        // b
        if (block.delta_b) next.b = *block.delta_b;
        // c
        if (block.missing_bitmap & 0x4) next.c = std::nullopt;
        else if (block.delta_c) next.c = *block.delta_c;
        output.push_back(next);
        prev = next;
    }
    return output;
}

int main() {
    // Some fake data, with "missing" and changes
    std::vector<Row> rows = {
        {1, 10, 100},
        {1, 10, 105},       // only c changed
        {2, 10, 105},       // only a changed
        {2, 15, std::nullopt}, // b changed, c missing
        {2, 15, 120},       // c reappears, value changes
        {2, 15, 120}        // repeat row (no delta)
    };

    std::cout << "Original rows:\n";
    for (const auto& r : rows) std::cout << r << "\n";

    // Delta encode
    auto deltas = delta_encode(rows);

    std::cout << "\nEncoded deltas:\n";
    for (const auto& d : deltas) d.print();

    // Reconstruct
    auto reconstructed = reconstruct_rows(rows.front(), deltas);

    std::cout << "\nReconstructed rows:\n";
    for (const auto& r : reconstructed) std::cout << r << "\n";

    // Check correctness
    bool ok = (rows == reconstructed);
    std::cout << "\nSuccess: " << (ok ? "YES" : "NO") << "\n";
    return ok ? 0 : 1;
}
