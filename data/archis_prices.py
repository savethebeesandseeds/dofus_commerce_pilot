import pandas as pd

# # Replace 'example.csv' with the path to your CSV file
# file_path = 'archis_precios.csv'
# df = pd.read_csv(file_path)

# # Display the first few rows of the dataframe
# print(df.head())

# sorted_df = df.sort_values(by=df.columns[0])

# # Save the sorted DataFrame to a new CSV file
# sorted_df.to_csv('archis_precios.csv', index=False)

# Replace 'example.csv' with the path to your CSV file
df_total = pd.read_csv('archis_total.csv')
df_precios = pd.read_csv('./output/archis_03172024.csv')

# Iterate over each row in df1
for index, row in df_total.iterrows():
    # Search for the key in df_precios
    matching_row = df_precios[df_precios['name'] == row['name']]
    
    # If a matching row is found in df_precios
    if not matching_row.empty:
        # Replace the value of the first column in df_total with the value from df_precios
        df_total.at[index, df_total.columns[0]] = matching_row.iloc[0][0]  # Assumes replacement with the first column from df_precios

# Optional: If you want to save the modified df_total to a new file
df_total = df_total.sort_values(by=df_total.columns[0])
df_total.to_csv('modified_total.csv', index=False)