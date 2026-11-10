async function loadProducts() {
    try {
        const response = await fetch("/api/products");
        const result = await response.json();

        if (!response.ok || !result.success) {
            throw new Error(result.message || "Could not load products");
        }

        console.log(result.products);
        // Render result.products inside your shop page here.
    } catch (error) {
        console.error(error);
    }
}

loadProducts();