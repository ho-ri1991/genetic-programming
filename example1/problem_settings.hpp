#ifndef GPLIB_EXAMPLE1_PROBLEM_SETTINGS
#define GPLIB_EXAMPLE1_PROBLEM_SETTINGS

#include <gp/utility/result.hpp>

namespace gp::example1 {

    template <typename RND>
    struct ProblemSettings {
        static constexpr const char* POPULATION_SIZE_FIELD = "problem.settings.population_size";
        static constexpr const char* MAX_TREE_DEPTH_FIELD = "problem.settings.max_tree_depth";
        static constexpr const char* EVOLUTION_NUM_FIELD = "problem.settings.evolution_num";
        static constexpr const char* MUTATION_RATE_FIELD = "problem.settings.mutation_rate";
        static constexpr const char* CROSSOVER_RATE_FIELD = "problem.settings.crossover_rate";
        static constexpr const char* RANDOM_SEED = "problem.settings.random_seed";
        std::size_t populationSize;
        std::size_t maxTreeDepth;
        std::size_t evolutionNum;
        double mutationRate;
        double crossoverRate;
        typename RND::result_type randomSeed;
    };

    template <typename RND>
    utility::Result<ProblemSettings<RND>> loadSettings(const std::string& name) {
        using namespace boost::property_tree;
        using namespace utility;
        ProblemSettings<RND> settings;
        std::ifstream fin(name);
        if(!fin.is_open())return result::err<ProblemSettings<RND>>("failed to load problem, file: " + name + " not found");
        ptree tree;
        try {
            xml_parser::read_xml(fin, tree);
        } catch (boost::property_tree::xml_parser_error& ex){
            return result::err<ProblemSettings<RND>>(std::string("failed to load problem, some error found in the xml file\n") + "line " + std::to_string(ex.line()) + ": " + ex.message());
        }

        return result::sequence(result::fromOptional(tree.get_optional<std::string>(ProblemSettings<RND>::POPULATION_SIZE_FIELD), "failed to load problem, population_size field not found"),
                                result::fromOptional(tree.get_optional<std::string>(ProblemSettings<RND>::MAX_TREE_DEPTH_FIELD), "failed to load problem, max_tree_depth field not found"),
                                result::fromOptional(tree.get_optional<std::string>(ProblemSettings<RND>::EVOLUTION_NUM_FIELD), "failed to load problem, evolution_num field not found"),
                                result::fromOptional(tree.get_optional<std::string>(ProblemSettings<RND>::MUTATION_RATE_FIELD), "failed to load problem, mutation_rate field not found"),
                                result::fromOptional(tree.get_optional<std::string>(ProblemSettings<RND>::CROSSOVER_RATE_FIELD), "failed to load problem, crossover_rate field not found"),
                                result::fromOptional(tree.get_optional<std::string>(ProblemSettings<RND>::RANDOM_SEED), "failed to load problem, random_seed field not found")
        ).flatMap([](auto&& propertyStrings){
            auto& populationNum = std::get<0>(propertyStrings);
            auto& maxTreeDepth = std::get<1>(propertyStrings);
            auto& evolutionNum = std::get<2>(propertyStrings);
            auto& mutationRate = std::get<3>(propertyStrings);
            auto& crossoverRate = std::get<4>(propertyStrings);
            auto& randomSeed = std::get<5>(propertyStrings);
            return result::sequence(result::tryFunction([&populationNum](){return boost::lexical_cast<decltype(ProblemSettings<RND>{}.populationSize)>(populationNum);}, "failed to load problem, invalid population_size field: " + populationNum),
                                    result::tryFunction([&maxTreeDepth](){return boost::lexical_cast<decltype(ProblemSettings<RND>{}.maxTreeDepth)>(maxTreeDepth);}, "failed to load problem, invalid max_tree_depth field: " + maxTreeDepth),
                                    result::tryFunction([&evolutionNum](){return boost::lexical_cast<decltype(ProblemSettings<RND>{}.evolutionNum)>(evolutionNum);}, "failed to load problem, invalid evoluion_num field: " + evolutionNum),
                                    result::tryFunction([&mutationRate](){return boost::lexical_cast<decltype(ProblemSettings<RND>{}.mutationRate)>(mutationRate);}, "failed to load problem, invalid mutation_rate field: " + mutationRate),
                                    result::tryFunction([&crossoverRate](){return boost::lexical_cast<decltype(ProblemSettings<RND>{}.crossoverRate)>(crossoverRate);}, "failed to load problem, invalid crossover_rate field: " + crossoverRate),
                                    result::tryFunction([&randomSeed](){return boost::lexical_cast<decltype(ProblemSettings<RND>{}.randomSeed)>(randomSeed);}, "failed to load problem, invalid random_seed: " + randomSeed));
        }).map([](auto&& prop){
            return ProblemSettings<RND>{std::get<0>(prop), std::get<1>(prop), std::get<2>(prop), std::get<3>(prop), std::get<4>(prop), std::get<5>(prop)};
        }).flatMap([](ProblemSettings<RND>&& settings){
            if(settings.mutationRate < 0) return result::err<ProblemSettings<RND>>("failed to load problem, mutation_rate must be positive");
            if(settings.crossoverRate < 0) return result::err<ProblemSettings<RND>>("failed to load problem, crossover_rate must be positive");
            if(1 < settings.mutationRate + settings.crossoverRate) return result::err<ProblemSettings<RND>>("failed to load problem, mutation_rate + crossover_rate < 1 must be fulfilled");
            return result::ok(std::move(settings));
        });
    }
}

#endif
