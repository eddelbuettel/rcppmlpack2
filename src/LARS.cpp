#include <RcppMLPACK.h>				// MLPACK, Rcpp and RcppArmadillo

#include <mlpack/methods/lars/lars.hpp> 		// particular algorithm used here

//' Run a lars/lasso/elasticNet regression
//'
//' This function performs a lars, lasso or elastic net regression.
//'
//' @title Run a lars / lasso / elasticNet regression
//' @param matX A matrix of explanatory variables, i.e. regressors
//' @param vecY A vector with the target variable
//' @param testPoints A matrix of test values to validate prediction
//' @param lambda1 A doube with l1-norm penalty regularization parameter
//' @param lambda2 A doube with l2-norm penalty regularization parameter
//' @param useCholesky A logical value indicating whether to use the Cholesky
//' decomposition when solving the linear system, else full Gram matrix is used.
//' @return A list with results
//' @examples
//'   data(trainSet)
//'   mat <- trainSet[, -5]
//'   y <- trainSet[,5]
//'   tst <- testSet[, -5]
//'   res <- LARS(mat, y, tst, 0.5, 0.5)
// [[Rcpp::export]]
Rcpp::List LARS(arma::mat& matX, arma::vec& vecY, const arma::mat& testPoints,
                const double lambda1, const double lambda2,
                bool useCholesky = false) {

    // Initialize the object.
    mlpack::regression::LARS lars(useCholesky, lambda1, lambda2);

    // Make sure y is oriented the right way.
    if (vecY.n_rows == 1)
        vecY = arma::trans(vecY);

    //if (vecY.n_cols > 1)
    //    Rcpp::stop("Only one column or row allowed in responses file!");

    if (vecY.n_elem != matX.n_rows)
        Rcpp::stop("Number of responses must be equal to number of rows of X!");

    arma::vec beta;
    //lars.Train(matX, matY.unsafe_col(0), beta, false /* do not transpose */);
    lars.Train(matX, vecY, beta, false /* do not transpose */);

    // TODO: test for having test data
    //Log::Info << "Regressing on test points." << endl;

    // Load test points.
    //mat testPoints = std::move(CLI::GetParam<arma::mat>("test"));

    // Make sure the dimensionality is right.  We haven't transposed, so, we
    // check n_cols not n_rows.
    if (testPoints.n_cols != lars.BetaPath().back().n_elem)
        Rcpp::stop("Dimensionality of test set (%d) "
                   "is not equal to the dimensionality of the model (%d)",
                   testPoints.n_cols, lars.BetaPath().back().n_elem);

    arma::vec predictions;
    lars.Predict(testPoints.t(), predictions, false);

    // Save test predictions.  One per line, so, don't transpose on save.
    //if (CLI::HasParam("output_predictions"))
    //    CLI::GetParam<arma::mat>("output_predictions") = std::move(predictions);

    return Rcpp::List::create(Rcpp::Named("predictions") = predictions);
}