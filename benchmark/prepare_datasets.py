#!/usr/bin/env python3
"""
Script to download and prepare diabetes, breast cancer, and California housing datasets from scikit-learn.
"""
import pandas as pd
from sklearn.datasets import load_diabetes, load_breast_cancer, fetch_california_housing
from sklearn.utils import check_random_state

def prepare_dataset(dataset, target, filename, random_seed=2025):
    """Prepare and save a dataset to CSV"""
    # Create a random state with the given seed
    rng = check_random_state(random_seed)
    
    # Randomly permute the dataset
    perm = rng.permutation(target.size)
    data = dataset[perm]
    target = target[perm]
    
    # Create a dataframe with the data and target
    df = pd.DataFrame(data)
    df['target'] = target
    
    # Save to CSV
    df.to_csv(filename, index=False)
    print(f"Saved {filename} with shape {df.shape}")

def main():
    # Prepare diabetes dataset for regression
    print("Downloading diabetes dataset...")
    diabetes = load_diabetes()
    prepare_dataset(diabetes.data, diabetes.target, 'diabetes.csv')
    
    # Prepare breast cancer dataset for classification
    print("Downloading breast cancer dataset...")
    cancer = load_breast_cancer()
    prepare_dataset(cancer.data, cancer.target, 'cancer.csv')
    
    # Prepare California housing dataset for regression
    print("Downloading California housing dataset...")
    housing = fetch_california_housing()
    prepare_dataset(housing.data, housing.target, 'housing.csv')
    
    print("Datasets prepared successfully!")

if __name__ == "__main__":
    main()
