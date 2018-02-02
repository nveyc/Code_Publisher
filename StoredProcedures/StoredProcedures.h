#pragma once
/////////////////////////////////////////////////////////////////////
// StoredProcedures.h - place to store pre-defined lambda queries  //
// ver 1.0                                                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017       //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package defines a single class, StoredProcedures that provides
* a container to store pre-defined query lambdas.
*
* Required Files:
* ---------------
*   Queries.h, Queries.cpp,
*
* Maintenance History:
*---------------------
* ver 1.0 : 22 Feb 2017
* - first release
*
* ToDo:
* -----
* This package is incomplete.  We need to move the testing to 
* StoredProcedures.cpp, and provide testing code in main.
*/

#include "../Queries/Queries.h"
#include <unordered_map>

namespace NoSQLDB
{
  template<typename Data>
  class StoredProcedures
  {
  public:
    using QueryType = typename Query<Data>::QueryType;

    StoredProcedures();
    QueryType getQuery(const std::string& qid);
    void accept(const std::string& qid, QueryType query);
    bool hasQuery(const std::string& qid);
  private:
    std::unordered_map<std::string, QueryType> procedures_;
  };
  //----< retrieve a query from query storage by id >----------------

  template<typename Data>
  typename StoredProcedures<Data>::QueryType StoredProcedures<Data>::getQuery(const std::string& qid)
  {
    if (hasQuery(qid))
      return procedures_[qid];
    return Query<Data>::QueryType();
  }
  //----< does query storage have a query with this id? >------------

  template<typename Data>
  bool StoredProcedures<Data>::hasQuery(const std::string& qid)
  {
    return (procedures_.find(qid) != procedures_.end());
  }
  //----< store or overwrite query in query store >------------------

  template<typename Data>
  void StoredProcedures<Data>::accept(const std::string& qid, QueryType query)
  {
    procedures_[qid] = query;
  }
  //----< initialize query store >-----------------------------------

  template<typename Data>
  StoredProcedures<Data>::StoredProcedures()
  {
    accept("getKeyIfExists", getKeyIfExists);
    accept("getChildren", getChildren);
    accept("getRexexKeyMatch", getRegexKeyMatch);
    accept("getRegexNameMatch", getRegexNameMatch);
    accept("getRegexCategoryMatch", getRegexCategoryMatch);
    accept("getRegexDataMatch", getRegexDataMatch);
    accept("getDateIntervalMatch", getDateIntervalMatch);
  }
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
  //----< return child keys of specified element >-------------------

  template<typename Data>
  void TestQuery2(const std::string& arg)
  {
    Query<Data>::QueryType getChildren = [&](const std::string& arg, Keys keys)
    {
      NoSQLDB::Keys returnKeys;
      std::cout << "\n    getChildren Query for \"" << arg << "\"";
      for (NoSQLDB::Key k : keys)
      {
        if (k == arg)
        {
          returnKeys = pDb->value(k).children;
          break;
        }
      }
      return returnKeys;
    };

    //std::cout << "\n\n  results of query #2:";
    //Keys keys = pDb->keys();
    //keys = getChildren(arg, keys);
    //if (keys.size() > 0)
    //{
    //  std::cout << "\n    found children : ";
    //  for (Key k : keys)
    //    std::cout << k << " ";
    //}
    //else
    //  std::cout << "\n    didn't find children for \"" << arg << "\"";
  }
  //----< return keys that match a specified regex pattern >---------

  template<typename Data>
  void TestQuery3(const std::string& arg)
  {
    Query<Data>::QueryType getRegexKeyMatch = [&](const std::string& arg, Keys keys)
    {
      NoSQLDB::Keys returnKeys;
      std::cout << "\n    key pattern match Query for \"" << arg << "\"";
      std::regex regx(arg);
      for (NoSQLDB::Key k : keys)
      {
        if (std::regex_match(k, regx))
        {
          returnKeys.push_back(k);
        }
      }
      return returnKeys;
    };

    //std::cout << "\n\n  results of query #3:";
    //Keys keys = pDb->keys();
    //keys = getRegexKeyMatch(arg, keys);
    //if (keys.size() > 0)
    //{
    //  std::cout << "\n    matches : ";
    //  for (Key k : keys)
    //    std::cout << k << " ";
    //}
    //else
    //  std::cout << "\n    didn't find match for \"" << arg << "\"";
  }
  //----< return elements with names that match a regex pattern >----

  template<typename Data>
  void TestQuery4(const std::string& arg)
  {
    Query<Data>::QueryType getRegexNameMatch = [&](const std::string& arg, Keys keys)
    {
      NoSQLDB::Keys returnKeys;
      std::cout << "\n    name pattern match Query for \"" << arg << "\"";
      std::regex regx(arg);
      for (NoSQLDB::Key k : keys)
      {
        Element<Data> elem = pDb->value(k);
        if (std::regex_match(static_cast<std::string>(elem.name), regx))
        {
          returnKeys.push_back(k);
        }
      }
      return returnKeys;
    };

    //std::cout << "\n\n  results of query #4:";
    //Keys keys = pDb->keys();
    //keys = getRegexNameMatch(arg, keys);
    //if (keys.size() > 0)
    //{
    //  std::cout << "\n    matches : ";
    //  for (Key k : keys)
    //    std::cout << k << " ";
    //}
    //else
    //  std::cout << "\n    didn't find match for \"" << arg << "\"";
  }
  //----< return elements with categories that match a pattern >-----

  template<typename Data>
  void TestQuery5(const std::string& arg)
  {
    Query<Data>::QueryType getRegexCategoryMatch = [&](const std::string& arg, Keys keys)
    {
      NoSQLDB::Keys returnKeys;
      std::cout << "\n    category pattern match Query for \"" << arg << "\"";
      std::regex regx(arg);
      for (NoSQLDB::Key k : keys)
      {
        Element<Data> elem = pDb->value(k);
        if (std::regex_match(static_cast<std::string>(elem.category), regx))
        {
          returnKeys.push_back(k);
        }
      }
      return returnKeys;
    };

    //std::cout << "\n\n  results of query #5:";
    //Keys keys = pDb->keys();
    //keys = getRegexCategoryMatch(arg, keys);
    //if (keys.size() > 0)
    //{
    //  std::cout << "\n    matches : ";
    //  for (Key k : keys)
    //    std::cout << k << " ";
    //}
    //else
    //  std::cout << "\n    didn't find match for \"" << arg << "\"";
  }
  //----< return elements with data that matches a pattern >---------

  template<typename Data>
  void TestQuery6(const std::string& arg)
  {
    Query<Data>::QueryType getRegexDataMatch = [&](const std::string& arg, Keys keys)
    {
      NoSQLDB::Keys returnKeys;
      std::cout << "\n    data pattern match Query for \"" << arg << "\"";
      std::regex regx(arg);
      for (NoSQLDB::Key k : keys)
      {
        Element<Data> elem = pDb->value(k);
        std::string data = elem.data;
        if (std::regex_match(data, regx))
        {
          returnKeys.push_back(k);
        }
      }
      return returnKeys;
    };

    //std::cout << "\n\n  results of query #6:";
    //Keys keys = pDb->keys();
    //keys = getRegexDataMatch(arg, keys);
    //if (keys.size() > 0)
    //{
    //  std::cout << "\n    matches : ";
    //  for (Key k : keys)
    //    std::cout << k << " ";
    //}
    //else
    //  std::cout << "\n    didn't find match for \"" << arg << "\"";
  }
  //----< return elements with DateTimes in an interval >------------

  template<typename Data>
  void TestQuery7(DateTime dt1, DateTime dt2)
  {
    Query<Data>::QueryType getDateIntervalMatch = [&](const std::string& arg, Keys keys)
    {
      NoSQLDB::Keys returnKeys;
      Element<Data> elem1 = pDb->value(keys[0]);
      DateTime dt;
      elem1.dateTime = (dt + DateTime::makeDuration(2, 0, 0, 0));
      pDb->saveValue(keys[0], elem1);
      std::cout << "\n    set dateTime of " << keys[0] << " to " << static_cast<DateTime>(elem1.dateTime).time();

      std::cout << "\n    date interval match Query for dates between:";
      std::cout << "\n    " << dt1.time() << " and " << dt2.time();

      for (NoSQLDB::Key k : keys)
      {
        Element<Data> elem = pDb->value(k);
        DateTime dt = elem.dateTime;
        std::cout << "\n    key : " << k << " has dateTime : " << dt.time();
        if (dt2 < dt && dt < dt1)
        {
          returnKeys.push_back(k);
        }
      }
      return returnKeys;
    };

    //std::cout << "\n\n  results of query #6:";
    //Keys keys = pDb->keys();
    //keys = getDateIntervalMatch("", keys);
    //if (keys.size() > 0)
    //{
    //  std::cout << "\n    matches : ";
    //  for (Key k : keys)
    //    std::cout << k << " ";
    //}
    //else
    //  std::cout << "\n    didn't find match for DateTime interval";
  }

}