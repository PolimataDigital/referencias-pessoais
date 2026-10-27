#include "../verificar.h"
#include <algorithm>
#include <charconv>
#include <limits>
#include <map>
#include <numeric>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

static std::optional<int> parse_int(std::string_view texto) {
    if (texto.empty()) return std::nullopt;
    int valor{};
    const char* fim = texto.data() + texto.size();
    auto [pos, erro] = std::from_chars(texto.data(), fim, valor);
    if (erro != std::errc{} || pos != fim) return std::nullopt;
    return valor;
}

static void preencher(std::span<int> dados, int valor) {
    for (int& x : dados) x = valor;
}

int main() {
    REF_CHECK(parse_int("-42") == -42);
    REF_CHECK(parse_int("0") == 0);
    REF_CHECK(!parse_int(""));
    REF_CHECK(!parse_int(" 7"));
    REF_CHECK(!parse_int("+7"));
    REF_CHECK(!parse_int("7 "));
    REF_CHECK(!parse_int("12abc"));
    REF_CHECK(!parse_int("999999999999999999999999999999999999999999999999"));
    REF_CHECK(parse_int(std::to_string(std::numeric_limits<int>::max())) ==
              std::numeric_limits<int>::max());
    REF_CHECK(parse_int(std::to_string(std::numeric_limits<int>::min())) ==
              std::numeric_limits<int>::min());
    std::vector<int> v{4, -1, 4, 2, 0};
    std::ranges::sort(v);
    REF_CHECK((v == std::vector<int>{-1, 0, 2, 4, 4}));
    REF_CHECK(std::lower_bound(v.begin(), v.end(), 4) == v.begin() + 3);
    REF_CHECK(std::upper_bound(v.begin(), v.end(), 4) == v.end());
    REF_CHECK(!std::binary_search(v.begin(), v.end(), 3));
    REF_CHECK(std::accumulate(v.begin(), v.end(), 0LL) == 9);
    v.erase(std::unique(v.begin(), v.end()), v.end());
    REF_CHECK((v == std::vector<int>{-1, 0, 2, 4}));
    REF_CHECK(std::erase_if(v, [](int x) { return x < 0; }) == 1);
    preencher(v, 8);
    REF_CHECK(std::ranges::all_of(v, [](int x) { return x == 8; }));
    std::vector<int> vazio;
    std::ranges::sort(vazio);
    REF_CHECK(std::lower_bound(vazio.begin(), vazio.end(), 1) == vazio.end());
    preencher(vazio, 0);
    std::map<std::string, int> frequencia;
    REF_CHECK(frequencia.find("a") == frequencia.end());
    REF_CHECK(frequencia.empty());
    ++frequencia["a"];
    REF_CHECK(frequencia.at("a") == 1 && frequencia.size() == 1);
    return ref_concluir();
}
