#include <string>
#include <vector>
#include <unordered_set>
#include <set>

//---------------------------------------------------------------------------
class JoinQuery
/// Class which provides answers to queries on lineitem orders and customer
{
   private:
    std::string lineitem;
    std::string orders;
    std::string customer;

   public:
  /**************************************************************************
   *  The constructor receives paths to data from the TPC-H benchmark.
   *  You can explore an example dataset from this benchmark on
   *  http://www.hyper-db.de/interface.html
   *  Furthermore, another example dataset is provided in
   *  test/data/tpch/sf0_001
   *
   *  Parameters to constructor:
   *     lineitem: path to a lineitem.tbl
   *     orders: path to a orders.tbl
   *     customer: path to a customer.tbl
   ************************************************************************/
   JoinQuery(std::string lineitem, std::string orders, std::string customer);

  /**************************************************************************
    *  Computes avg(l_quantity) for the following query.
    *
    *  select avg(l_quantity)
    *  from lineitem, orders, customer
    *  where
    *   l_orderkey = o_orderkey and
    *   o_custkey = c_custkey and
    *   c_mktsegment = <segmentParam>
    *
    *  where the tables lineitem, orders and customer are those identified by
    *the paths given in the constructor.
    *
    *  Parameters:
    *     segmentParam: string to use instead of <segmentParam> in query
    *
    *  Returns: avg(l_quantity) * 100
    *     In the data files, l_quantity is of fixed point type with
    *     two digits right of the point. Thus avg(l_quantity) shouls
    *     also have two digits right of the point. Therefore
    *     avg(l_quantity) * 100 is the same with the point dropped.
    ************************************************************************/
   size_t avg(std::string segmentParam);
   /**
    *
    * @param segmentParam
    * @return all ids of customer which have the same segment as segmentParam
    */
   std::unordered_set<int>getCustomerIds(std::string segmentParam);
   /**
    *
    * get all orderkeys where the custkey is in customerIds
    *
    */
    std::unordered_set<int> getOrderIds(std::unordered_set<int>customerIds);
    /**
     *
     * @param orderIds
     * @return all quantities of table lineitem belonging to the orders from the parameter orderIds
     */
    std::multiset<float>getLineitemQuantities(std::unordered_set<int>orderIds);
   /// Returns line count of given file
   size_t lineCount(std::string rel);
};
//---------------------------------------------------------------------------
