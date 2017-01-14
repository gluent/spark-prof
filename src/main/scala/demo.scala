import com.typesafe.scalalogging.LazyLogging

import org.apache.spark.{SparkConf,SparkContext}
import org.apache.spark.sql.{SparkSession,Row}
import org.apache.spark.sql.types._

import scala.util.Try


object SparkProf extends LazyLogging {

  def main(args: Array[String]) = {
    val conf = new SparkConf().setAppName("spark_prof")
    val sc = new SparkContext(conf)

    // set to true to run in SparkSQL/DataFrame mode, false to run in RDD mode
    val useSparkSql = true
    //
    // number of times to repeat final map(row(yearIndex)).reduce(+)
    val lastQueryRepeat = 10

    // ms to sleep in between lastQueryRepeat
    val sleepMs = 5000

    val yearIndex = 20

    val lines = sc.textFile("/tmp/simple_data.csv")
    val stringFields = lines.map(line => line.split(","))
    val fullFieldLength = stringFields.first.length
    val completeFields = stringFields.filter(fields => fields.length == fullFieldLength)
    val data = completeFields.map(fields => fields.patch(yearIndex, Array(Try(fields(yearIndex).toInt).getOrElse(0)), 1))


    def log(msg: String) = logger.info(s"\n\n============================================= $msg =============================================")



    if(useSparkSql) {
      val ss = SparkSession.builder.config(conf).getOrCreate()

      import ss.implicits._

      val fields = completeFields.first
        .map(fieldName => fieldName match {
          case "Year" => StructField(fieldName, IntegerType, nullable = true)
          case _ => StructField(fieldName, StringType, nullable = true)
        })
      val schema = StructType(fields)

      val dataFrame = ss.createDataFrame(data.map(d => Row(d: _*)), schema)

      log("cache entire data-frame in memory")
      dataFrame.cache()

      log("run map(length).max to populate cache")
      println(dataFrame.map(r => r.length).reduce((l1, l2) => Math.max(l1, l2)))
      
      log("re-run map(length).max to sanity check")
      println(dataFrame.map(r => r.length).reduce((l1, l2) => Math.max(l1, l2)))

      for(loopIndex <- 1 to lastQueryRepeat) {
        log(s"$loopIndex] sleep for $sleepMs...")
        Thread.sleep(sleepMs)

        log(s"$loopIndex] run map(row(yearIndex)).reduce(+)")
        println(dataFrame.map(r => r(yearIndex).asInstanceOf[Int]).reduce((y1, y2) => y1 + y2))
      }

    } else {
      log("cache entire RDD in memory")
      data.cache()

      log("re-run map(length).max to sanity check")
      println(data.map(r => r.length).reduce((l1, l2) => Math.max(l1, l2)))

      log("re-run map(length).max to sanity check")
      println(data.map(r => r.length).reduce((l1, l2) => Math.max(l1, l2)))

      for(loopIndex <- 1 to lastQueryRepeat) {
        log(s"$loopIndex] sleep for $sleepMs...")
        Thread.sleep(sleepMs)

        log(s"$loopIndex] run map(row(yearIndex)).reduce(+)")
        println(data.map(d => d(yearIndex).asInstanceOf[Int]).reduce((y1, y2) => y1 + y2))
      }
    }
  }
}
