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

/**
 * Class which contains the configuration of the application.
 * @note Singleton class, use AppConfig::getInstance() to access the instance.
 * @author SD
 */
class AppConfig {
   UniformUnitGenerator generator;
   static std::unique_ptr<AppConfig> instance;
   AppConfig(unsigned seed) : generator(seed) {}

public:
   /**
    * Returns the instance of the AppConfig, if already initialized.
    * @throws std::logic_error if the instance isn't initialized.
    * @see Use AppConfig::initialize() to initialize first.
    * @author SD
    */
   static AppConfig& getInstance() {
      if (!instance) {
         throw std::logic_error("AppConfig has not been initialized");
      }
      return *instance;
   }

   /**
    * Creates AppConfig instance with the given seed.
    * @param seed Used for the random number generator
    * @see After calling this, use AppConfig::getInstance() to access the instance.
    * @author SD
    */
   static void initialize(unsigned seed) {
      if (!instance) {
         instance.reset(new AppConfig(seed));
      }
   }

   // Delete copy constructor and assignment operator
   AppConfig(const AppConfig&) = delete;
   AppConfig& operator=(const AppConfig&) = delete;
   AppConfig() = delete; // Prevent default constructor

   /**
    * Returns the UniformUnitGenerator reference for generating random numbers.
    * @author SD
    */
   UniformUnitGenerator& getUnif() { return generator; }

   /**
    * Creates a new generator with the given seed.
    * @param seed Seed for the new generator
    * @author SD
    */
   void setGenerator(int seed) { generator = UniformUnitGenerator(seed); }
};
}
#endif