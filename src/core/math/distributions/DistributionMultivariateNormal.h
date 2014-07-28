//
//  DistributionMultivariateNormal.h
//  revbayes
//
//  Created by Nicolas Lartillot on 2014-03-28.
//  Copyright (c) 2014 revbayes team. All rights reserved.
//

#ifndef __revbayes__DistributionMultivariateNormal__
#define __revbayes__DistributionMultivariateNormal__

#include <iostream>

#endif /* defined(__revbayes__DistributionMultivariateNormal__) */


#include <vector>
#include "MatrixRealSymmetric.h"

namespace RevBayesCore {
    
    class RandomNumberGenerator;
    
    
    namespace RbStatistics {
        
        namespace MultivariateNormal {

            // parameterization in terms of the covariance matrix sigma
            double                      pdfCovariance(const std::vector<double>& mu, const MatrixRealSymmetric& sigma, const std::vector<double>& z);        /*!< MultivariateNormal(a[]) probability density */
            double                      lnPdfCovariance(const std::vector<double>& mu, const MatrixRealSymmetric& sigma, const std::vector<double>& z);      /*!< MultivariateNormal(a[]) log_e probability density */
            std::vector<double>         rvCovariance(const std::vector<double>& mu, const MatrixRealSymmetric& sigma, RandomNumberGenerator& rng);           /*!< MultivariateNormal(a[]) random variable */

            // parameterization in terms of the precision matrix (i.e. inverse covariance matrix) omega
            double                      pdfPrecision(const std::vector<double>& mu, const MatrixRealSymmetric& omega, const std::vector<double>& z);        /*!< MultivariateNormal(a[]) probability density */
            double                      lnPdfPrecision(const std::vector<double>& mu, const MatrixRealSymmetric& omega, const std::vector<double>& z);      /*!< MultivariateNormal(a[]) log_e probability density */
            std::vector<double>         rvPrecision(const std::vector<double>& mu, const MatrixRealSymmetric& omega, RandomNumberGenerator& rng);           /*!< MultivariateNormal(a[]) random variable */
        }
    }
}

