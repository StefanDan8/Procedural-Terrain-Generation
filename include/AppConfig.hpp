#ifndef _APP_CONFIG
#define _APP_CONFIG

#include <memory>
#include <random>
#include <stdexcept>
namespace perlin {

/// @author SD
/// @brief Random number generator for Unif[0.0, 1.0]
class UniformUnitGenerator {
   public:
   UniformUnitGenerator(unsigned seed) : generator(seed), distribution(0.0, 1.0) {}

   double get() {
      return distribution(generator);
   }

   private:
   std::mt19937 generator;
   std::uniform_real_distribution<double> distribution;
};

// class AppConfig {
//    public:
//    static AppConfig* Instance() {
//       static AppConfig s;
//       return &s;
//    }
//    AppConfig(const AppConfig&) = delete;
//    void operator=(const AppConfig&) = delete;

//    private:
//    AppConfig() = default;
//    UniformUnitGenerator generator;
// };

class AppConfig {
   public:
   static AppConfig& getInstance() {
      if (!instance) {
         throw std::logic_error("AppConfig has not been initialized");
      }
      return *instance;
   }
   static void initialize(unsigned seed) {
      if (!instance) {
         instance.reset(new AppConfig(seed));
      }
   }

   AppConfig(const AppConfig&) = delete;
   AppConfig& operator=(const AppConfig&) = delete;
   AppConfig() = delete;

   UniformUnitGenerator& getUnif() {
      return generator;
   }

   void setGenerator(int seed) {
      generator = UniformUnitGenerator(seed);
   }

   private:
   UniformUnitGenerator generator;
   AppConfig(unsigned seed) : generator(seed) {}
   static std::unique_ptr<AppConfig> instance;
};
}
#endif