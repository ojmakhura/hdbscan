library("dbscan")
library("dict")

#make it a full path
fname <- "/home/junior/programming/phd/hdbscan/test_datasets/mydata.csv"
mPts <- 5

sps <- strsplit(fname, "/")[[1]]
sps[length(sps)] <- paste(c("clusters/", "r_", mPts, "_", sps[length(sps)]), collapse="")
newName <- paste(sps, collapse="/")

dt <- read.csv(fname, header = FALSE)

if(is.na(dt[length(dt)])) {
  dt <- dt[1:length(dt)-1]
}

cl <- hdbscan(dt, minPts = mPts)

cmap <- dict()
i <- 1
for(l in cl$cluster) {
  
  not_present <- cmap$get(l, FALSE) == FALSE
  
  if(not_present) {
    lst <- list(i)
    cmap$set(l, lst)
  } else {
    lst <- cmap$get(l)
    lst[length(lst) + 1] <- i
    cmap$set(l, lst)
    #break
  }
  
  i <- i + 1
  
}

for(k in cmap$keys()) {
  tmp <- toString(c(k, cmap$get(k)))
  print(tmp)
  write(tmp, file = newName, append = TRUE)
  print("----")
}

print(cl$outlier_scores)
