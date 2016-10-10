/**
 * @class Obvious2DClient
 * @brief Interface for specifying object-oriented key-press callback methods
 */
class Obvious2DClient
{

public:

  /**
   * Constructor
   */
  Obvious2DClient(){};

  /**
   * Destructor
   */
  virtual ~Obvious2DClient(){};

  /**
   *
   * @param key
   */
  virtual void keyboardCallback(char key) = 0;

};
