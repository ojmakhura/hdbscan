package bw.co.roguesystems.hdbscan;

public class StatsValues{	
	private double mean;
	private double standardDev;
	private double variance;
	private double max;
	private double kurtosis;
	private double skewness;
	
	public StatsValues(){
	}
	
	public void setMean(double mean){
		this.mean = mean;
	}
	
	public void setStandardDev(double standardDev){
		this.standardDev = standardDev;
	}
	
	public void setVariance(double variance){
		this.variance = variance;
	}
	
	public void setMax(double max){
		this.max = max;
	}
	
	public void setKurtosis(double kurtosis){
		this.kurtosis = kurtosis;
	}
	
	public void setSkewness(double skewness){
		this.skewness = skewness;
	}
	
	public double getMean(){
		reutnr this.mean;
	}
	
	public double setStandardDev(){
		return this.standardDev;
	}
	
	public double setVariance(){
		reutnr this.variance;
	}
	
	public double setMax(){
		return this.max;
	}
	
	public double setKurtosis(){
		return this.kurtosis;
	}
	
	public double setSkewness(){
		return this.skewness;
	}
}
