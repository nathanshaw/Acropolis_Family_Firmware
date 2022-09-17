// main.ck
me.dir() + "analysis_prototyping.ck" => string analysisString;
me.dir() + "Rhythms.ck" => string rhythmsString;

Machine.add(rhythmsString);
Machine.add(analysisString);

while(1){1::second=>now;}