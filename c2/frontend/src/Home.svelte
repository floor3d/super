<script>
  export let authenticated;
  let data = [];
  let isLoading = true;

  // Fetch the data from the backend when the component mounts
  async function fetchData() {
    try {
      const response = await fetch('http://localhost:5000/api/implants');
      data = await response.json();
    } catch (error) {
      console.error('Error fetching data:', error);
    } finally {
      isLoading = false;
    }
  }

  // Call fetchData when the component is created
  fetchData();
</script>

<div>
  <h1>Home</h1>
  {#if authenticated}
    {#if isLoading}
      <p>Loading...</p>
    {:else}
      <!-- Display the data in a table once it has been fetched -->
      <table>
        <thead>
          <tr>
            <th>ID</th>
            <th>Name</th>
            <th>Email</th>
          </tr>
        </thead>
        <tbody>
          {#each data as implant}
            <tr>
              <td>{implant.id}</td>
              <td>{implant.firstconnected}</td>
              <td>{implant.isactive}</td>
            </tr>
          {/each}
        </tbody>
      </table>
    {/if}
  {:else}
    <p>Please log in.</p>
  {/if}
</div>
