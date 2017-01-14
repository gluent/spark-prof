val spark = "org.apache.spark" %% "spark-core" % "2.0.1" % "provided"
val streaming = "org.apache.spark" % "spark-streaming_2.11" % "2.0.1" % "provided"
val spark_sql = "org.apache.spark" % "spark-sql_2.11" % "2.0.1" % "provided"
val spark_hive = "org.apache.spark" % "spark-hive_2.11" % "2.0.1" % "provided"

val logback = "ch.qos.logback" % "logback-classic" % "1.1.7"
val scala_logging = "com.typesafe.scala-logging" %% "scala-logging" % "3.5.0"

lazy val root = (project in file(".")).
  settings(
    name := "spark_prof",
    version := "1.0",
    scalaVersion := "2.11.8",
    libraryDependencies ++= Seq(spark, streaming, spark_sql, spark_hive, logback, scala_logging)
  )
