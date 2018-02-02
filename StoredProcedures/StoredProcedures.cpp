// StoredProcedures.cpp

#include "StoredProcedures.h"

#ifdef TEST_STOREDPROCEDURES

//----< query does key exist? >------------------------------------

template<typename Data>
void TestQuery1(const std::string& arg)
{
  Query<Data>::QueryType getKeyIfExists = [](const std::string& arg, Keys keys)
  {
    NoSQLDB::Keys returnKeys;
    std::cout << "\n    getKeyIfExists Query for \"" << arg << "\"";
    for (NoSQLDB::Key k : keys)
    {
      if (k == arg)
      {
        returnKeys.push_back(static_cast<std::string>(k));
        break;
      }
    }
    return returnKeys;
  };

  //std::cout << "\n  results of query #1:";
  //Keys keys = pDb->keys();
  //keys = getKeyIfExists(arg, keys);
  //if (keys.size() == 1)
  //  std::cout << "\n    found key \"" << arg << "\"";
  //else
  //  std::cout << "\n    didn't find key \"" << arg << "\"";
}

int main()
{

}
#endif
