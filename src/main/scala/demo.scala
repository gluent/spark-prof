import com.typesafe.scalalogging.LazyLogging

import org.apache.spark.{SparkConf,SparkContext}
import org.apache.spark.sql.SparkSession

import scala.util.Try


object SparkProf extends LazyLogging {

  def main(args: Array[String]) = {
    val conf = new SparkConf().setAppName("spark_prof").setMaster("local[2]")
    val sc = new SparkContext(conf)

    val year_index = 20

    val lines = sc.textFile("/tmp/simple_data.csv")
    val stringFields = lines.map(line => line.split(",")).filter(fields => fields.length > year_index)
    val data = stringFields.map(fields => fields.patch(year_index, Array(Try(fields(year_index).toInt).getOrElse(0)), 1))

    //data.cache()

    //println(data.map(d => d.length).max)

    val sleep_ms = 5000
    logger.info(s"Sleeping for $sleep_ms...")
    Thread.sleep(sleep_ms)
    logger.info("Resuming")

    //data.map(d => d(year_index)).take(10).foreach(println)

    println(data.map(d => d(year_index).asInstanceOf[Int]).reduce((y1, y2) => y1 + y2))

  }
}
